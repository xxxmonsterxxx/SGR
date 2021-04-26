#pragma once

#include "utils.h"

class PipelineManager;
class SwapChainManager;

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

public:
	static RenderPassManager* get();
};
