#pragma once

#include "utils.h"
#include "DrawCommand.h"

class SGR;
class RenderPassManager;
class SwapChainManager;

class CommandManager {
private:
	friend class SGR;
	friend class RenderPassManager;
	friend class SwapChainManager;

	CommandManager();
	~CommandManager();
	CommandManager(const CommandManager&) = delete;
	CommandManager operator=(CommandManager&) = delete;

	static CommandManager* instance;

	static CommandManager* get();

	VkCommandPool commandPool = VK_NULL_HANDLE;
	sgrErrCode initCommandPool();

	std::vector<VkCommandBuffer> commandBuffers;
	sgrErrCode initCommandBuffers();
	sgrErrCode endInitCommandBuffers();
	bool buffersEnded = false;

	std::vector<Command*> commands;
	sgrErrCode executeCommands();
	void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);
};
