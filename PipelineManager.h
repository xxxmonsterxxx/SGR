#pragma once

#include "utils.h"

class SGR;
class CommandManager;
class SwapChainManager;

class PipelineManager {
private:
	friend class SGR;
	friend class CommandManager;
	friend class SwapChainManager;

	PipelineManager();
	~PipelineManager();
	PipelineManager(const PipelineManager&) = delete;
	PipelineManager operator=(PipelineManager&) = delete;

	static PipelineManager* instance;

	sgrErrCode init();

	VkPipelineLayout pipelineLayout;
	VkPipeline pipeline;

public:
	static PipelineManager* get();
};
