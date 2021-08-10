#include <SGR.h>
#include <iostream>

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
	vertexBindingDescription.stride = sizeof(Sgr2DVertex);
	vertexBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	std::vector<VkVertexInputBindingDescription> vertexBindingDescriptions{ vertexBindingDescription };
	return vertexBindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> createAttrDescr()
{
	VkVertexInputAttributeDescription positionDescr;
	positionDescr.binding = 0;
	positionDescr.location = 0;
	positionDescr.format = VK_FORMAT_R32G32_SFLOAT;
	positionDescr.offset = offsetof(Sgr2DVertex, position);

	VkVertexInputAttributeDescription colorDescr;
	colorDescr.binding = 0;
	colorDescr.location = 1;
	colorDescr.format = VK_FORMAT_R32G32B32_SFLOAT;
	colorDescr.offset = offsetof(Sgr2DVertex, color);

	std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions;
	vertexAttributeDescriptions.push_back(positionDescr);
	vertexAttributeDescriptions.push_back(colorDescr);
	return vertexAttributeDescriptions; 
}



SGR sgr_object1;

SgrTime_t lastDraw = SgrTime::now();
SgrDynamicUniformBufferObject rectangles;
SgrUniformBufferObject ubo;

void updateData() {
	if (getSgrTimeDuration(lastDraw, SgrTime::now()) > 0.1) {
		glm::vec2* texCoord = (glm::vec2*)((uint64_t)rectangles.data + sizeof(glm::mat4));
		texCoord->x += 0.111;
		if (texCoord->x >= 1)
			texCoord->x = 0.055;

		lastDraw = SgrTime::now();
		sgr_object1.updateDynamicUniformBuffer(rectangles);
	}

	sgr_object1.updateUniformBuffer(ubo);
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
	std::vector<Sgr2DVertex> obMeshVertices = {
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
	};

	std::string objectName1 = "triangle";
	std::vector<uint16_t> obMeshIndices1 = { 0, 1, 2 };
	std::vector<Sgr2DVertex> obMeshVertices1 = {
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}}
	};

	std::string obShaderVert = "shaders/vertex.spv";
	std::string obShaderFrag = "shaders/fragment.spv";
	std::string obShaderFragColor = "shaders/colorFragment.spv";

	SgrUniformBufferObject objectsUBO;
	std::string ob1Texture = "textures/man.png";
	std::string ob2Texture = "textures/tree.png";

	// new object layout creating done. Next step - setup this data to SGR and add command to draw.

	std::vector<VkDescriptorSetLayoutBinding> setLayoutBinding = createDescriptorSetLayoutBinding();

	std::vector<VkVertexInputBindingDescription> bindInpDescr = createBindingDescr();
	std::vector<VkVertexInputAttributeDescription> attDescr = createAttrDescr();


	struct InstanceData {
		glm::mat4 model;
		glm::vec2 texCoord;
		glm::vec2 texSize;
	};

	rectangles.instnaceCount = 2;
	rectangles.instanceSize = sizeof(InstanceData);
	MemoryManager::createDynamicUniformMemory(rectangles);
	SgrBuffer* instanceUBO = nullptr;
	SgrErrCode resultCreateBuffer = MemoryManager::get()->createDynamicUniformBuffer(instanceUBO, rectangles.dataSize);
	if (resultCreateBuffer != sgrOK)
		return resultCreateBuffer;

	sgr_object1.setupDynamicUniformBuffer(instanceUBO);

	SgrErrCode resultAddNewObject = sgr_object1.addNewObjectGeometry(objectName, obMeshVertices, obMeshIndices, obShaderVert, obShaderFrag, bindInpDescr, attDescr, setLayoutBinding);
	if (resultAddNewObject != sgrOK)
		return resultAddNewObject;

	resultAddNewObject = sgr_object1.addNewObjectGeometry(objectName1, obMeshVertices1, obMeshIndices1, obShaderVert, obShaderFragColor, bindInpDescr, attDescr, setLayoutBinding);
	if (resultAddNewObject != sgrOK)
		return resultAddNewObject;

	SgrBuffer* uboBuffer = nullptr;
	resultCreateBuffer = MemoryManager::get()->createUniformBuffer(uboBuffer, sizeof(SgrUniformBufferObject));
	if (resultCreateBuffer != sgrOK)
		return resultCreateBuffer;
	
	SgrImage* texture1 = nullptr;
	SgrErrCode resultCreateTextureImage = TextureManager::createTextureImage(ob1Texture, texture1);
	if (resultCreateTextureImage != sgrOK)
		return resultCreateTextureImage;

	SgrImage* texture2 = nullptr;
	resultCreateTextureImage = TextureManager::createTextureImage(ob2Texture, texture2);
	if (resultCreateTextureImage != sgrOK)
		return resultCreateTextureImage;


	sgr_object1.addObjectInstance("man","rectangle",0*rectangles.dynamicAlignment);
	sgr_object1.addObjectInstance("tree","triangle",1*rectangles.dynamicAlignment);

	std::vector<void*> objectData;
	objectData.push_back((void*)(uboBuffer));
	objectData.push_back((void*)(texture1));
	objectData.push_back((void*)(instanceUBO));
	sgr_object1.writeDescriptorSets("man", objectData);

	std::vector<void*>objectData2;
	objectData2.push_back((void*)(uboBuffer));
	objectData2.push_back((void*)(texture2));
	objectData2.push_back((void*)(instanceUBO));
	sgr_object1.writeDescriptorSets("tree", objectData2);


	sgr_object1.setupUniformBuffer(uboBuffer);

	glm::mat4* model = (glm::mat4*)((uint64_t)rectangles.data);
	*model = glm::mat4(1.f);
	*model = glm::translate(*model, glm::vec3(-0.2f, 0.35f, 0.f));
	*model = glm::scale(*model,glm::vec3(0.3f,0.3f,1.f));
	glm::vec2* texCoord = (glm::vec2*)((uint64_t)rectangles.data + sizeof(glm::mat4));
	texCoord->x = 0.055;
	texCoord->y = 0.125;
	glm::vec2* texSize = (glm::vec2*)((uint64_t)rectangles.data + sizeof(glm::vec2) + sizeof(glm::mat4));
	texSize->x = 0.222;
	texSize->y = 0.5;

	model = (glm::mat4*)((uint64_t)rectangles.data + rectangles.dynamicAlignment);
	*model = glm::mat4(1.f);
	*model = glm::scale(*model,glm::vec3(1.f,1.f,1.f));
	texCoord = (glm::vec2*)((uint64_t)rectangles.data + sizeof(glm::mat4) + rectangles.dynamicAlignment);
	texCoord->x = 0.5f;
	texCoord->y = 0.5f;
	texSize = (glm::vec2*)((uint64_t)rectangles.data + sizeof(glm::vec2) + sizeof(glm::mat4) + rectangles.dynamicAlignment);
	texSize->x = 2.f;
	texSize->y = 2.f;

	sgr_object1.updateDynamicUniformBuffer(rectangles);

	ubo.view = glm::mat4(1.f);
	// ubo.view = glm::rotate(ubo.view, glm::radians(90.f), glm::vec3(0, 0, 1));
	ubo.proj = glm::mat4(1.f);
	sgr_object1.updateUniformBuffer(ubo);

	if (sgr_object1.drawObject("man") != sgrOK)
		return 100;

	if (sgr_object1.drawObject("tree") != sgrOK)
		return 200;

	sgr_object1.setUpdateFunction(updateData);
	while (sgr_object1.isSGRRunning()) {
		SgrErrCode resultDrawFrame = sgr_object1.drawFrame();
		if (resultDrawFrame != sgrOK)
			return resultDrawFrame;
	}

	SgrErrCode resultSGRDestroy = sgr_object1.destroy();
	return resultSGRDestroy;
}