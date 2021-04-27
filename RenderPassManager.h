#pragma once

#include "utils.h"

class PipelineManager;
class SwapChainManager;
class CommandManager;

class RenderPassManager {
private:
	RenderPassManager();
	~RenderPassManager();
	RenderPassManager(const RenderPassManager&) = delete;
	RenderPassManager operator=(RenderPassManager&) = delete;

	static RenderPassManager* instance;

	sgrErrCode init();

	VkRenderPass renderPass;

	friend class PipelineManager;
	friend class SwapChainManager;
	friend class CommandManager;

public:
	static RenderPassManager* get();
};
