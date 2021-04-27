#pragma once

#include "utils.h"

class SGR;
class RenderPassManager;

class CommandManager {
private:
	CommandManager();
	~CommandManager();
	CommandManager(const CommandManager&) = delete;
	CommandManager operator=(CommandManager&) = delete;

	static CommandManager* instance;

	static CommandManager* get();

	VkCommandPool commandPool;
	sgrErrCode initCommandPool();

	std::vector<VkCommandBuffer> commandBuffers;
	sgrErrCode initCommandBuffers();
	void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);
	sgrErrCode endInitCommandBuffers();

	bool buffersEnded = false;

	friend class SGR;
	friend class RenderPassManager;
};
