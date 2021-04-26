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

	friend class SGR;
	friend class RenderPassManager;
};
