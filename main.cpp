#include "SGR.h"
#include "iostream"

// Wrapper functions for aligned memory allocation
	// There is currently no standard for this in C++ that works across all platforms and vendors, so we abstract this
void* SGRalignedAlloc(size_t size, size_t alignment)
{
	void* data = nullptr;
#if defined(_MSC_VER) || defined(__MINGW32__)
	data = _aligned_malloc(size, alignment);
#else
	int res = posix_memalign(&data, alignment, size);
	if (res != 0)
		data = nullptr;
#endif
	return data;
}

void SGRalignedFree(void* data)
{
#if	defined(_MSC_VER) || defined(__MINGW32__)
	_aligned_free(data);
#else
	free(data);
#endif
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

	SgrErrCode resultAddNewObject = sgr_object1.addNewTypeObject(objectName, obMeshVertices, obMeshIndices, obShaderVert, obShaderFrag, setLayoutBinding, bindInpDescr, attDescr);
	if (resultAddNewObject != sgrOK)
		return resultAddNewObject;


	SgrBuffer* uboBuffer = nullptr;
	SgrErrCode resultCreateBuffer = MemoryManager::get()->createUniformBuffer(uboBuffer, sizeof(UniformBufferObject));
	if (resultCreateBuffer != sgrOK)
		return resultCreateBuffer;
	
	SgrImage* texture = nullptr;
	SgrErrCode resultCreateTextureImage = TextureManager::createTextureImage(ob1Texture, texture);
	if (resultCreateTextureImage != sgrOK)
		return resultCreateTextureImage;

	SgrDynamicUniformBufferObject rectangles;
	rectangles.instnaceCount = 2;
	rectangles.instanceSize = sizeof(glm::mat4);
	MemoryManager::createDynamicUniformMemory(rectangles);
	SgrBuffer* instanceUBO = nullptr;
	resultCreateBuffer = MemoryManager::get()->createDynamicUniformBuffer(instanceUBO, rectangles.dataSize);
	if (resultCreateBuffer != sgrOK)
		return resultCreateBuffer;

	std::vector<void*> objectData;
	objectData.push_back((void*)(uboBuffer));
	objectData.push_back((void*)(texture));
	objectData.push_back((void*)(instanceUBO));

	sgr_object1.updateDescriptorSets("rectangle", objectData);

	sgr_object1.setupUniformBuffers("rectangle", uboBuffer, instanceUBO);

	glm::mat4* object1 = (glm::mat4*)((uint64_t)rectangles.data);
	*object1 = glm::rotate(glm::mat4(1.0f), glm::radians(9.f), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4* object2 = (glm::mat4*)((uint64_t)rectangles.data + rectangles.dynamicAlignment);
	*object2 = glm::translate(glm::mat4(1.f), glm::vec3(-0.2f, -0.2f, 0.f));
	sgr_object1.updateDynamicUniformBuffer("rectangle", rectangles);

	/*sgr_object1.updateUniformBuffer("rectange", ubo);*/

	sgr_object1.drawObject("rectangle", 2);

	while (sgr_object1.isSGRRunning()) {
		SgrErrCode resultDrawFrame = sgr_object1.drawFrame();
		if (resultDrawFrame != sgrOK)
			return resultDrawFrame;
	}

	SgrErrCode resultSGRDestroy = sgr_object1.destroy();
	return resultSGRDestroy;
}