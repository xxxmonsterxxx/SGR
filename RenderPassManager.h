#pragma once

#include "utils.h"

class PipelineManager;

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

public:
	static RenderPassManager* get();
};
