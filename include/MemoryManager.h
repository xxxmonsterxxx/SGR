#pragma once

#include "utils.h"
#include "SwapChainManager.h"

class SGR;
class TextureManager;

struct SgrBuffer {
	VkBuffer vkBuffer;
	VkDeviceMemory bufferMemory;
	VkDeviceSize size;
	VkDeviceSize blockRange; // for dynamic uniform buffer
};

struct SgrGlobalUBO {
	glm::mat4 view = glm::mat4(1.f);
	glm::mat4 proj = glm::mat4(1.f);
};

struct SgrInstancesUBO {
	SgrBuffer* ubo = nullptr;
	void* data = nullptr;
	size_t instanceSize = 0;
	size_t instnaceCount = 0;
	size_t dynamicAlignment = 0;
	size_t dataSize = 0;
};

class MemoryManager {
	friend class SGR;
	friend class TextureManager;
	friend class SwapChainManager;

	MemoryManager();
	~MemoryManager();
	MemoryManager(const MemoryManager&) = delete;
	MemoryManager& operator=(const MemoryManager&) = delete;

	static MemoryManager* instance;

	static SgrErrCode findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags props, uint32_t& findedProps);
	SgrErrCode createBuffer(SgrBuffer*& buffer, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
	SgrErrCode createStagingBufferWithData(SgrBuffer*& buffer, VkDeviceSize size, void* data);
	SgrErrCode createBufferUsingStaging(SgrBuffer*& buffer, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, void* data);
	void	   copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	static void destroyBuffer(SgrBuffer* buffer);

	SgrErrCode createVertexBuffer(SgrBuffer*& buffer, VkDeviceSize size, void* vertexData);
	SgrErrCode createIndexBuffer(SgrBuffer*& buffer, VkDeviceSize size, void* indexData);

	static void copyBufferToImage(SgrBuffer* buffer, SgrImage* image);
	static void copyDataToBuffer(SgrBuffer* buffer, void* data);

	std::vector<SgrBuffer*> allocatedBuffers;

public:
	static MemoryManager* get();
	static SgrErrCode createDynamicUniformMemory(SgrInstancesUBO& dynamicUBO);
	SgrErrCode createDynamicUniformBuffer(SgrBuffer*& buffer, VkDeviceSize size, VkDeviceSize blockRange = 0);

	SgrErrCode destroyAllocatedBuffers();
};
