#include "SGR.h"
#include "iostream"

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

	std::vector<VkDescriptorSetLayoutBinding> layoutBindings = { uboLayoutBinding, samplerLayoutBinding };
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

	VkVertexInputAttributeDescription textureDescr;
	textureDescr.binding = 0;
	textureDescr.location = 2;
	textureDescr.format = VK_FORMAT_R32G32_SFLOAT;
	textureDescr.offset = offsetof(Sgr2DVertex, texCoord);

	std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions;
	vertexAttributeDescriptions.push_back(positionDescr);
	vertexAttributeDescriptions.push_back(colorDescr);
	vertexAttributeDescriptions.push_back(textureDescr);
	return vertexAttributeDescriptions;
}

int main()
{
	// This source code is example for using SGR library

	SGR sgr_object1;
	
	SgrErrCode resultSGRInit = sgr_object1.init();
	if (resultSGRInit != sgrOK) {
		return resultSGRInit;
	}

	/*SgrErrCode resultInitSimpleTestObject = sgr_object1.addToFrameSimpleTestObject();
	if (resultInitSimpleTestObject != sgrOK)
		return resultInitSimpleTestObject;*/

	// scheme to setup 2D-object layout:
	// 1. setup geometry (mesh)
	// 2. setup minimum two shaders: vertex and fragment
	
	// then we need to setup uniform buffer pointer for all objects same mesh
	// setup texture for each object

	// create geometry layout (mesh)

	std::string objectName = "rectangle";
	std::vector<uint16_t> obMeshIndices = { 0, 1, 2, 2, 3, 0 };
	std::vector<Sgr2DVertex> obMeshVertices = {
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
	};

	std::string obShaderVert = "SGRShaders/vertex.spv";
	std::string obShaderFrag = "SGRShaders/fragment.spv";

	UniformBufferObject objectsUBO;
	std::string ob1Texture = "Textures/test_texture.jpg";
	std::string ob2Texture = "Textures/test_texture2.jpg";

	// new object layout creating done. Next step - setup this data to SGR and add command to draw.

	std::vector<VkDescriptorSetLayoutBinding> setLayoutBinding = createDescriptorSetLayoutBinding();

	std::vector<VkVertexInputBindingDescription> bindInpDescr = createBindingDescr();
	std::vector<VkVertexInputAttributeDescription> attDescr = createAttrDescr();
	UniformBufferObject dynamicBuffer;
	UniformBufferObject ubo;

	SgrErrCode resultAddNewObject = sgr_object1.addNewTypeObject(objectName, obMeshVertices, obMeshIndices, obShaderVert, obShaderFrag, setLayoutBinding, bindInpDescr, attDescr, sizeof(glm::mat4));
	if (resultAddNewObject != sgrOK)
		return resultAddNewObject;

	std::vector<void*> objectData;
	SgrBuffer* uboBuffer = nullptr;
	SgrErrCode resultCreateBuffer = MemoryManager::get()->createUniformBuffer(uboBuffer, sizeof(UniformBufferObject));
	if (resultCreateBuffer != sgrOK)
		return resultCreateBuffer;

	objectData.push_back((void*)(uboBuffer));

	SgrImage* texture = nullptr;
	SgrErrCode resultCreateTextureImage = TextureManager::createTextureImage(ob1Texture, texture);
	if (resultCreateTextureImage != sgrOK)
		return resultCreateTextureImage;
	objectData.push_back((void*)(texture));

	sgr_object1.updateDescriptorSets("rectangle", objectData);
	sgr_object1.drawObject("rectangle");
	//sgr_object1.updateUniformBuffer("rectange", ubo);
	//sgr_object1.updateDynamicUniformBuffer("rectange", dynamicBuffer);



	while (sgr_object1.isSGRRunning()) {
		SgrErrCode resultDrawFrame = sgr_object1.drawFrame();
		if (resultDrawFrame != sgrOK)
			return resultDrawFrame;
	}

	SgrErrCode resultSGRDestroy = sgr_object1.destroy();
	return resultSGRDestroy;
}