#pragma once

#include "utils.h"

class SGR;
class CommandManager;

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
	friend class CommandManager;

public:
	static PipelineManager* get();
};
