#pragma once

#include "utils.h"
#include "Command.h"

class SGR;
class RenderPassManager;
class SwapChainManager;
class MemoryManager;

class CommandManager {
private:
	friend class SGR;
	friend class RenderPassManager;
	friend class SwapChainManager;
	friend class MemoryManager;

	CommandManager();
	~CommandManager();
	CommandManager(const CommandManager&) = delete;
	CommandManager operator=(CommandManager&) = delete;

	static CommandManager* instance;

	static CommandManager* get();

	VkCommandPool commandPool = VK_NULL_HANDLE;
	SgrErrCode initCommandPool();

	std::vector<VkCommandBuffer> commandBuffers;
	SgrErrCode initCommandBuffers();
	SgrErrCode endInitCommandBuffers();
	bool buffersEnded = false;

	std::vector<Command*> commands;
	SgrErrCode executeCommands();
	void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);
	void bindVertexBuffer(std::vector<VkBuffer> vertexBuffers, VkDeviceSize* offsets = nullptr);
	void bindIndexBuffer(VkBuffer indexBuffer);
	void drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t verteOffset, uint32_t firstInstance);
};
