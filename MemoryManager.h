#pragma once

#include "utils.h"
#include "SwapChainManager.h"

class SGR;
class TextureManager;
class SwapChainManager;

struct SgrBuffer {
	VkBuffer vkBuffer;
	VkDeviceMemory bufferMemory;
	VkDeviceSize size;
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

	std::vector<SgrBuffer*> allocatedBuffers;


public:
	static MemoryManager* get();
	SgrErrCode createUniformBuffer(SgrBuffer*& buffer, VkDeviceSize size);
	static SgrErrCode createDynamicUniformMemory(SgrDynamicUniformBufferObject& dynamicUBO);
	SgrErrCode createDynamicUniformBuffer(SgrBuffer*& buffer, VkDeviceSize size);
};
