#include <SGR.h>
#include <iostream>

#include <stdio.h>

#define STB_TRUETYPE_IMPLEMENTATION
	#include "stb_truetype.h"

bool getFontData(std::string font_path, unsigned char* &fontPixels, stbtt_bakedchar* &backedChars, uint32_t &width, uint32_t &height)
{
	FILE* font_ttf = fopen(font_path.c_str(), "rb");
	if (!font_ttf) {
		printf("Font file not found! :(\n");
		return false;
	}

	long file_size;
	if (fseek(font_ttf, 0, SEEK_END) !=0) {
		printf("File is corrupted\n");
		return false;
	} else {
		file_size = ftell(font_ttf);
		fseek(font_ttf, 0, SEEK_SET);
	}

	unsigned char* font_data = (unsigned char*)malloc(file_size);
	if(fread(font_data, 1, file_size, font_ttf) != file_size) {
		printf("Real time corrupted file\n");
		return false;
	};
	fclose(font_ttf);

	int bitmapWidth = 512;
	int bitmapHeight = 512;
	width = bitmapWidth;
	height = bitmapHeight;
	fontPixels = (unsigned char*)malloc(bitmapWidth*bitmapHeight);
	const int char_number_to_bake = 96;
	backedChars = (stbtt_bakedchar*)malloc(char_number_to_bake*sizeof(stbtt_bakedchar));

	int result = stbtt_BakeFontBitmap(font_data, 0, 64, fontPixels, bitmapWidth, bitmapHeight, 32, char_number_to_bake, backedChars);
	free(font_data);

	return true;
}


void getLetterFontData(char letter, stbtt_bakedchar* fontData, glm::vec4& mesh, glm::vec4& text)
{
	stbtt_aligned_quad charData;
	float xstart=0, ystart=0;
	stbtt_GetBakedQuad(fontData, 512, 512, letter - 32, &xstart, &ystart, &charData, 1);

	mesh = glm::vec4(charData.x0, charData.y0, charData.x1, charData.y1);
	mesh*=2.f/512; // conversion from bitmap size to frame [-1; 1] size
	text = glm::vec4(charData.s0, charData.t0, charData.s1, charData.t1);
}


float getSgrTimeDuration(SgrTime_t start, SgrTime_t end)
{
	return std::chrono::duration<float, std::chrono::seconds::period>(end - start).count();
}

std::vector<VkDescriptorSetLayoutBinding> createDescriptorSetLayoutBinding()
{
	VkDescriptorSetLayoutBinding uboLayoutBinding;
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	samplerLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding instanceUBOLayoutBinding{};
	instanceUBOLayoutBinding.binding = 2;
	instanceUBOLayoutBinding.descriptorCount = 1;
	instanceUBOLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	instanceUBOLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	instanceUBOLayoutBinding.pImmutableSamplers = nullptr;

	std::vector<VkDescriptorSetLayoutBinding> layoutBindings = { uboLayoutBinding,  samplerLayoutBinding, instanceUBOLayoutBinding };
	return layoutBindings;
}

std::vector<VkVertexInputBindingDescription> createBindingDescr()
{
	VkVertexInputBindingDescription vertexBindingDescription;
	vertexBindingDescription.binding = 0;
	vertexBindingDescription.stride = sizeof(SgrVertex);
	vertexBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	std::vector<VkVertexInputBindingDescription> vertexBindingDescriptions{ vertexBindingDescription };
	return vertexBindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> createAttrDescr()
{
	VkVertexInputAttributeDescription positionDescr;
	positionDescr.binding = 0;
	positionDescr.location = 0;
	positionDescr.format = VK_FORMAT_R32G32B32_SFLOAT;
	positionDescr.offset = 0;

	std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions;
	vertexAttributeDescriptions.push_back(positionDescr);
	return vertexAttributeDescriptions; 
}



SGR sgr_object1;

SgrTime_t frame_dr;
SgrTime_t lastDraw = SgrTime::now();
SgrTime_t lastTextureChange = SgrTime::now();

// data structure for instance uses shader presenter as "instance shader"
struct InstanceData {
	glm::mat4 model;
	glm::vec4 color;
	glm::vec2 deltaText;
	glm::vec2 startMesh;
	glm::vec2 startText;
};

SgrInstancesUniformBufferObject rectangles;
SgrGlobalUniformBufferObject ubo;

void updateData() {
	if (getSgrTimeDuration(lastDraw, SgrTime::now()) > 0.1) {
		InstanceData* iData = (InstanceData*)((uint64_t)rectangles.data + 0*rectangles.dynamicAlignment);
		glm::vec2* texCoord = &iData->startText;
		texCoord->x += 0.111;
		if (texCoord->x >= 1)
			texCoord->x = 0;

		iData->model = glm::translate(iData->model, glm::vec3(0, 0, -0.09));

		lastDraw = SgrTime::now();
	}

	sgr_object1.updateGlobalUniformBufferObject(ubo);
	sgr_object1.updateInstancesUniformBufferObject(rectangles);
};

int main()
{
	// This source code is example for using SGR library

	
	SgrErrCode resultSGRInit = sgr_object1.init();
	if (resultSGRInit != sgrOK) {
		return resultSGRInit;
	}


	std::string executablePath = getExecutablePath();
	if (executablePath.length() == 0)
		return 11;
	std::string resourcePath = executablePath + "/Resources";

	sgr_object1.setApplicationLogo(resourcePath + "/logo/Logo64.png");

	// scheme to setup 2D-object layout:
	// 1. setup geometry (mesh)
	// 2. setup minimum two shaders: vertex and fragment
	
	// then we need to setup uniform buffer pointer for all objects same mesh
	// setup texture for each object

	// create geometry layout (mesh)

	std::string objectName = "rectangle";
	std::vector<uint16_t> obMeshIndices = { 0, 1, 2, 2, 3, 0 };
	std::vector<SgrVertex> obMeshVertices = {{-0.5f, -0.5f, 0},
											 { 0.5f, -0.5f, 0},
											 { 0.5f,  0.5f, 0},
											 {-0.5f,  0.5f, 0}};

	std::string objectName1 = "triangle";
	std::vector<uint16_t> obMeshIndices1 = { 0, 1, 2 };
	std::vector<SgrVertex> obMeshVertices1 = {{-0.5f, -0.5f, 0},
											  { 0.5f, -0.5f, 0},	
											  { 0.5f,  0.5f, 0}};



	std::string obShaderVert = resourcePath + "/shaders/vertInstanceSh.spv";
	std::string obShaderFrag = resourcePath + "/shaders/fragTextureSh.spv";
	std::string obShaderFragColor = resourcePath + "/shaders/fragColorSh.spv";

	SgrGlobalUniformBufferObject objectsUBO;
	std::string ob1Texture = resourcePath + "/textures/man.png";
	std::string roadT = resourcePath + "/textures/road.png";
	std::string ob2Texture = resourcePath + "/textures/tree.png";

	// new object layout creating done. Next step - setup this data to SGR and add command to draw.

	std::vector<VkDescriptorSetLayoutBinding> setLayoutBinding = createDescriptorSetLayoutBinding();
	std::vector<VkVertexInputBindingDescription> bindInpDescr = createBindingDescr();
	std::vector<VkVertexInputAttributeDescription> attDescr = createAttrDescr();

	rectangles.instnaceCount = 4;
	rectangles.instanceSize = sizeof(InstanceData);
	if (MemoryManager::createDynamicUniformMemory(rectangles) != sgrOK)
		return 0;
	SgrBuffer* instanceUBO = nullptr;
	SgrErrCode resultCreateBuffer = MemoryManager::get()->createDynamicUniformBuffer(instanceUBO, rectangles.dataSize, rectangles.dynamicAlignment);
	if (resultCreateBuffer != sgrOK)
		return resultCreateBuffer;

	sgr_object1.setupInstancesUniformBufferObject(instanceUBO);

	SgrBuffer* uboBuffer = nullptr;
	resultCreateBuffer = MemoryManager::get()->createUniformBuffer(uboBuffer, sizeof(SgrGlobalUniformBufferObject));
	if (resultCreateBuffer != sgrOK)
		return resultCreateBuffer;

	SgrErrCode resultAddNewObject = sgr_object1.addNewObjectGeometry(objectName, obMeshVertices, obMeshIndices, obShaderVert, obShaderFrag, true, bindInpDescr, attDescr, setLayoutBinding);
	if (resultAddNewObject != sgrOK)
		return resultAddNewObject;
	
	SgrImage* texture1 = nullptr;
	SgrErrCode resultCreateTextureImage = TextureManager::createTextureImage(ob1Texture, texture1);
	if (resultCreateTextureImage != sgrOK)
		return resultCreateTextureImage;

	SgrImage* road = nullptr;
	resultCreateTextureImage = TextureManager::createTextureImage(roadT, road);
	if (resultCreateTextureImage != sgrOK)
		return resultCreateTextureImage;

	SgrImage* textImage = nullptr;
	unsigned char* fontPixels = nullptr;
	stbtt_bakedchar* fontData;
	uint32_t pixelsWidth, pixelsHeight;
	std::string font_path = resourcePath + "/fonts/times new roman.ttf";
	getFontData(font_path, fontPixels, fontData, pixelsWidth, pixelsHeight);
	resultCreateTextureImage = TextureManager::createFontTextureImage(fontPixels, pixelsWidth, pixelsHeight, textImage);

	sgr_object1.addObjectInstance("man","rectangle",0*rectangles.dynamicAlignment);

	std::vector<void*> objectData;
	objectData.push_back((void*)(uboBuffer));
	objectData.push_back((void*)(texture1));
	objectData.push_back((void*)(instanceUBO));
	sgr_object1.writeDescriptorSets("man", objectData);



	resultAddNewObject = sgr_object1.addNewObjectGeometry(objectName1, obMeshVertices1, obMeshIndices1, obShaderVert, obShaderFragColor, false, bindInpDescr, attDescr, setLayoutBinding);
	if (resultAddNewObject != sgrOK)
		return resultAddNewObject;

	SgrImage* texture2 = nullptr;
	resultCreateTextureImage = TextureManager::createTextureImage(ob2Texture, texture2);
	if (resultCreateTextureImage != sgrOK)
		return resultCreateTextureImage;

	sgr_object1.addObjectInstance("tree","triangle",1*rectangles.dynamicAlignment);

	std::vector<void*>objectData2;
	objectData2.push_back((void*)(uboBuffer));
	objectData2.push_back((void*)(texture2));
	objectData2.push_back((void*)(instanceUBO));
	sgr_object1.writeDescriptorSets("tree", objectData2);


	//-------

	sgr_object1.addObjectInstance("road","rectangle",2*rectangles.dynamicAlignment);

	std::vector<void*> objectData3;
	objectData3.push_back((void*)(uboBuffer));
	objectData3.push_back((void*)(road));
	objectData3.push_back((void*)(instanceUBO));
	sgr_object1.writeDescriptorSets("road", objectData3);

	glm::vec4 meshLetter;
	glm::vec4 textLetter;
	getLetterFontData('S', fontData,meshLetter,textLetter);
	std::vector<SgrVertex> letterMesh = {{meshLetter.x, meshLetter.y, 0},
										 {meshLetter.z, meshLetter.y, 0},
										 {meshLetter.z, meshLetter.w, 0},
										 {meshLetter.x, meshLetter.w, 0}};
	resultAddNewObject = sgr_object1.addNewObjectGeometry("letterMesh", letterMesh, obMeshIndices, obShaderVert, obShaderFrag, true, bindInpDescr, attDescr, setLayoutBinding);
	if (resultAddNewObject != sgrOK)
		return resultAddNewObject;
	glm::vec2 letStartMesh(meshLetter.x, meshLetter.y);
	glm::vec2 letStartText(textLetter.x, textLetter.y);
	glm::vec2 deltaText;
	deltaText.x = (textLetter.z - textLetter.x) / (meshLetter.z - meshLetter.x);
	deltaText.t = (textLetter.w - textLetter.y) / (meshLetter.w - meshLetter.y);
	
	sgr_object1.addObjectInstance("letter", "letterMesh", 3*rectangles.dynamicAlignment);

	std::vector<void*> objectData4;
	objectData4.push_back((void*)(uboBuffer));
	objectData4.push_back((void*)(textImage));
	objectData4.push_back((void*)(instanceUBO));
	sgr_object1.writeDescriptorSets("letter", objectData4);

	if (sgr_object1.drawObject("man") != sgrOK)
		return 100;

	if (sgr_object1.drawObject("tree") != sgrOK)
		return 200;

	if (sgr_object1.drawObject("road") != sgrOK)
		return 300;

	if (sgr_object1.drawObject("letter") != sgrOK)
		return 400;


	sgr_object1.setupGlobalUniformBufferObject(uboBuffer);


// MAN
	InstanceData* iData = (InstanceData*)((uint64_t)rectangles.data + 0*rectangles.dynamicAlignment);
	iData->model = glm::mat4(1.f);
	iData->model = glm::translate(iData->model, glm::vec3(0, 0, 0));
	iData->startMesh = glm::vec2(-0.5,-0.5);
	iData->startText = glm::vec2(0,0);
	iData->deltaText.x = (0.111 - 0) / (0.5 - -0.5);
	iData->deltaText.y = (0.250 - 0) / (0.5 - -0.5);

// TRIANGLE
	iData = (InstanceData*)((uint64_t)rectangles.data + 1*rectangles.dynamicAlignment);
	iData->model = glm::mat4(1.f);
	iData->model = glm::translate(iData->model,glm::vec3(1,-0.5,-3));
	iData->model = glm::scale(iData->model,glm::vec3(1.f,1.f,1.f));
	iData->color.r = 1;
	iData->color.g = 0;
	iData->color.b = 0;

// ROAD
	iData = (InstanceData*)((uint64_t)rectangles.data + 2*rectangles.dynamicAlignment);
	iData->model = glm::mat4(1.f);
	iData->model = glm::translate(iData->model,glm::vec3(-2,-2,-5));
	iData->model = glm::scale(iData->model,glm::vec3(1.f,1.f,1.f));
	iData->startMesh = glm::vec2(-0.5,-0.5);
	iData->startText = glm::vec2(0,0);
	iData->deltaText.x = (1 - 0) / (0.5 - -0.5);
	iData->deltaText.y = (1 - 0) / (0.5 - -0.5);

// LETTER
	InstanceData* current = (InstanceData*)((uint64_t)rectangles.data + 3*rectangles.dynamicAlignment);
	current->model = glm::mat4(1.f);
	current->model = glm::translate(current->model,glm::vec3(0,1.5,-3));
	current->startMesh = letStartMesh;
	current->startText = letStartText;
	current->deltaText = deltaText;

	sgr_object1.updateInstancesUniformBufferObject(rectangles);
	ubo.view = glm::translate(ubo.view, glm::vec3(0, 0, -1));
	ubo.proj = glm::perspective(45.f, 1.f/1.f, 0.1f, 100.f);
	sgr_object1.updateGlobalUniformBufferObject(ubo);

	sgr_object1.setUpdateFunction(updateData);
	while (1) {
		if (!sgr_object1.isSGRRunning())
			break;

		SgrErrCode resultDrawFrame = sgr_object1.drawFrame();

		if (getSgrTimeDuration(lastTextureChange, SgrTime::now()) > 5) {
			std::vector<void*> objectDataRoad;
			objectDataRoad.push_back((void*)(uboBuffer));
			objectDataRoad.push_back((void*)(texture1));
			objectDataRoad.push_back((void*)(instanceUBO));
			sgr_object1.writeDescriptorSets("road", objectDataRoad);
		}

		if (getSgrTimeDuration(lastTextureChange, SgrTime::now()) > 7) {
			std::vector<void*> objectDataRoad;
			objectDataRoad.push_back((void*)(uboBuffer));
			objectDataRoad.push_back((void*)(road));
			objectDataRoad.push_back((void*)(instanceUBO));
			sgr_object1.writeDescriptorSets("road", objectDataRoad);
		}

		if (resultDrawFrame != sgrOK)
			return resultDrawFrame;
	}

	SgrErrCode resultSGRDestroy = sgr_object1.destroy();
	return resultSGRDestroy;
}