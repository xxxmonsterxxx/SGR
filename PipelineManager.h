#pragma once

#include "utils.h"

class SGR;

class PipelineManager {
private:
	PipelineManager();
	~PipelineManager();
	PipelineManager(const PipelineManager&) = delete;
	PipelineManager operator=(PipelineManager&) = delete;

	static PipelineManager* instance;

	sgrErrCode init();

	VkPipelineLayout pipelineLayout;
	VkPipeline pipeline;

	friend class SGR;

public:
	static PipelineManager* get();
};
