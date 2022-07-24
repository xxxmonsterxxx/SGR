#include <SGR.h>
#include <iostream>

#include <stdio.h>

#define STB_TRUETYPE_IMPLEMENTATION
	#include "../modules/stb/stb_truetype.h"

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
	unsigned char* fontPixels1 = (unsigned char*)malloc(bitmapWidth*bitmapHeight);
	unsigned char fontPixels2[512*512];
	const int char_number_to_bake = 96;
	backedChars = (stbtt_bakedchar*)malloc(char_number_to_bake*sizeof(stbtt_bakedchar));

	int result = stbtt_BakeFontBitmap(font_data, 0, 64, fontPixels, bitmapWidth, bitmapHeight, 32, char_number_to_bake, backedChars);
	free(font_data);

	return true;
}


void getLetterFontData(char letter, stbtt_bakedchar* fontData, glm::vec2& fontCoord, glm::vec2& fontSize)
{
	stbtt_aligned_quad charData;
	float xstart, ystart;
	stbtt_GetBakedQuad(fontData, 512, 512, (uint32_t)letter - 32, &xstart, &ystart, &charData, 1);

	float xFont = (charData.s1-charData.s0)*0.5f;
	float yFont = (charData.t1-charData.t0)*0.5f;

	fontCoord = glm::vec2(charData.s0+xFont,charData.t0+yFont);
	fontSize = glm::vec2(charData.s1-charData.s0,charData.t1-charData.t0);
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
SgrInstancesUniformBufferObject rectangles;
SgrGlobalUniformBufferObject ubo;

void updateData() {
	if (getSgrTimeDuration(lastDraw, SgrTime::now()) > 0.1) {
		glm::vec2* texCoord = (glm::vec2*)((uint64_t)rectangles.data + sizeof(glm::mat4));
		texCoord->x += 0.111;
		if (texCoord->x >= 1)
			texCoord->x = 0.166;

		glm::mat4* model = (glm::mat4*)((uint64_t)rectangles.data);
		*model = glm::translate(*model, glm::vec3(0, 0, -0.09));

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
											 { 0.5f, 0.5f,  0},
											 {-0.5f, 0.5f,  0}};

	std::string objectName1 = "triangle";
	std::vector<uint16_t> obMeshIndices1 = { 0, 1, 2 };
	std::vector<SgrVertex> obMeshVertices1 = {{-0.5f, -0.5f, 0},
											  { 0.5f, -0.5f, 0},	
											  { 0.5f, 0.5f,  0}};

	std::string resourcePath = getExecutablePath() + "/Resources";

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

	struct InstanceData {
		glm::mat4 model;
		glm::vec2 texCoord;
		glm::vec2 texSize;
		glm::vec3 color;
	};

	rectangles.instnaceCount = 4;
	rectangles.instanceSize = sizeof(InstanceData);
	MemoryManager::createDynamicUniformMemory(rectangles);
	SgrBuffer* instanceUBO = nullptr;
	SgrErrCode resultCreateBuffer = MemoryManager::get()->createDynamicUniformBuffer(instanceUBO, rectangles.dataSize);
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



	resultAddNewObject = sgr_object1.addNewObjectGeometry(objectName1, obMeshVertices1, obMeshIndices1, obShaderVert, obShaderFragColor, true, bindInpDescr, attDescr, setLayoutBinding);
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


	sgr_object1.addObjectInstance("LetterH", "rectangle", 3*rectangles.dynamicAlignment);

	std::vector<void*> objectData4;
	objectData4.push_back((void*)(uboBuffer));
	objectData4.push_back((void*)(textImage));
	objectData4.push_back((void*)(instanceUBO));
	sgr_object1.writeDescriptorSets("LetterH", objectData4);


	if (sgr_object1.drawObject("man") != sgrOK)
		return 100;

	if (sgr_object1.drawObject("tree") != sgrOK)
		return 200;

	if (sgr_object1.drawObject("road") != sgrOK)
		return 300;

	if (sgr_object1.drawObject("LetterH") != sgrOK)
		return 400;

	//------------

	sgr_object1.setupGlobalUniformBufferObject(uboBuffer);

	glm::mat4* model = (glm::mat4*)((uint64_t)rectangles.data);
	*model = glm::mat4(1.f);
	*model = glm::translate(*model, glm::vec3(0, 0, 0));
	glm::vec2* texCoord = (glm::vec2*)((uint64_t)rectangles.data + sizeof(glm::mat4));
	texCoord->x = 0.055;
	texCoord->y = 0.125;
	glm::vec2* texSize = (glm::vec2*)((uint64_t)rectangles.data + sizeof(glm::vec2) + sizeof(glm::mat4));
	texSize->x = 0.222;
	texSize->y = 0.5;

	model = (glm::mat4*)((uint64_t)rectangles.data + rectangles.dynamicAlignment);
	*model = glm::mat4(1.f);
	*model = glm::translate(*model,glm::vec3(0,0,-3));
	*model = glm::scale(*model,glm::vec3(1.f,1.f,1.f));
	glm::vec3* color = (glm::vec3*)((uint64_t)rectangles.data + sizeof(glm::vec2) + sizeof(glm::vec2) + sizeof(glm::mat4) + 1*rectangles.dynamicAlignment);
	color->r = 0;
	color->g = 0;
	color->b = 1;

	model = (glm::mat4*)((uint64_t)rectangles.data + 2*rectangles.dynamicAlignment);
	*model = glm::mat4(1.f);
	*model = glm::translate(*model,glm::vec3(-1,0,-5));
	*model = glm::scale(*model,glm::vec3(1.f,1.f,1.f));
	texCoord = (glm::vec2*)((uint64_t)rectangles.data + sizeof(glm::mat4) + 2*rectangles.dynamicAlignment);
	texCoord->x = 0.5f;
	texCoord->y = 0.5f;
	texSize = (glm::vec2*)((uint64_t)rectangles.data + sizeof(glm::vec2) + sizeof(glm::mat4) + 2*rectangles.dynamicAlignment);
	texSize->x = 2.f;
	texSize->y = 2.f;

	model = (glm::mat4*)((uint64_t)rectangles.data + 3*rectangles.dynamicAlignment);
	*model = glm::mat4(1.f);
	*model = glm::translate(*model,glm::vec3(1,0,-2));
	*model = glm::scale(*model,glm::vec3(1.f,1.f,1.f));
	texCoord = (glm::vec2*)((uint64_t)rectangles.data + sizeof(glm::mat4) + 3*rectangles.dynamicAlignment);
	glm::vec2 fontC,fontS;
	getLetterFontData('s', fontData, fontC,fontS);
	texCoord->x = fontC.x;
	texCoord->y = fontC.y;
	texSize = (glm::vec2*)((uint64_t)rectangles.data + sizeof(glm::vec2) + sizeof(glm::mat4) + 3*rectangles.dynamicAlignment);
	texSize->x = fontS.x*2;
	texSize->y = fontS.y*2;

	sgr_object1.updateInstancesUniformBufferObject(rectangles);
	ubo.view = glm::translate(ubo.view, glm::vec3(0, 0, -1));
	ubo.proj = glm::perspective(45.f, 1.f/1.f, 0.1f, 100.f);
	sgr_object1.updateGlobalUniformBufferObject(ubo);

	sgr_object1.setUpdateFunction(updateData);
	while (1) {
		if (!sgr_object1.isSGRRunning())
			break;

		SgrErrCode resultDrawFrame = sgr_object1.drawFrame();

		if (resultDrawFrame != sgrOK)
			return resultDrawFrame;
	}

	SgrErrCode resultSGRDestroy = sgr_object1.destroy();
	return resultSGRDestroy;
}