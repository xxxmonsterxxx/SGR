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

public:
	struct SgrPipeline {
		std::string name = "empty";
		VkPipelineLayout* pipelineLayout;
		VkPipeline* pipeline;
	};

	static PipelineManager* get();

private:
	PipelineManager();
	~PipelineManager();
	PipelineManager(const PipelineManager&) = delete;
	PipelineManager operator=(PipelineManager&) = delete;
	static PipelineManager* instance;

	std::vector<SgrPipeline> pipelines;

	SgrErrCode createAndAddPipeline(std::string name, ShaderManager::SgrShader objectShaders, DescriptorManager::SgrDescriptorInfo descriptorInfo);
	SgrErrCode createPipeline(ShaderManager::SgrShader objectShaders, 
							  DescriptorManager::SgrDescriptorInfo descriptorInfo,
							  SgrPipeline& sgrPipeline);
	SgrErrCode destroyAllPipelines();
	SgrErrCode reinitAllPipelines();
	SgrPipeline* getPipelineByName(std::string name);
};
