#pragma once

#include "utils.h"

#include "ShaderManager.h"
#include "DescriptorManager.h"

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

	struct SgrPipeline {
		std::string name;
		VkPipelineLayout pipelineLayout;
		VkPipeline pipeline;
	};

	std::vector<SgrPipeline> pipelines;

	SgrErrCode createPipeline(  std::string name, VkRenderPass renderPass,
								ShaderManager::SgrShader objectShaders, 
								DescriptorManager::SgrDescriptorInfo descriptorInfo);
	SgrErrCode destroyPipeline(std::string name);
	SgrErrCode destroyAllPipelines();
	SgrErrCode reinitAllPipelines();
	SgrPipeline getPipelineByName(std::string name);
public:
	static PipelineManager* get();
};
