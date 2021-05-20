#pragma once

#include "utils.h"

class SGR;
class CommandManager;
class SwapChainManager;
class BindDescriptorSetCommand;

class PipelineManager {
	friend class SGR;
	friend class CommandManager;
	friend class SwapChainManager;
	friend class BindDescriptorSetCommand;

private:

	PipelineManager();
	~PipelineManager();
	PipelineManager(const PipelineManager&) = delete;
	PipelineManager operator=(PipelineManager&) = delete;

	static PipelineManager* instance;

	SgrErrCode init();

	VkPipelineLayout pipelineLayout;
	VkPipeline pipeline;

	VkVertexInputBindingDescription vertexBindingDescription;
	std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions;

public:
	static PipelineManager* get();
};
