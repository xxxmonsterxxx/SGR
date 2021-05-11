#pragma once

#include "utils.h"

class SGR;

struct SgrBuffer {
	VkBuffer vkBuffer;
	VkDeviceMemory bufferMemory;
};

class MemoryManager {
	friend class SGR;

	MemoryManager();
	~MemoryManager();
	MemoryManager(const MemoryManager&) = delete;
	MemoryManager& operator=(const MemoryManager&) = delete;

	static MemoryManager* instance;

	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags props);
	SgrErrCode createBuffer(SgrBuffer*& buffer, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
	SgrErrCode createBufferUsingStaging(SgrBuffer*& buffer, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, void* data);
	void	   destroyBuffer(SgrBuffer* buffer);
	void	   copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	SgrErrCode createVertexBuffer(SgrBuffer*& buffer, VkDeviceSize size, void* vertexData);
	SgrErrCode createIndexBuffer(SgrBuffer*& buffer, VkDeviceSize size, void* indexData);

	std::vector<SgrBuffer*> allocatedBuffers;


public:

	static MemoryManager* get();
};
