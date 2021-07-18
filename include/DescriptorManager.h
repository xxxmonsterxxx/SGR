#pragma once

#include "utils.h"
#include "SwapChainManager.h"

class SGR;
class PipelineManager;

class DescriptorManager {
	friend class SGR;
	friend class PipelineManager;

private:
	DescriptorManager();
	DescriptorManager(const DescriptorManager&) = delete;
	DescriptorManager& operator=(const DescriptorManager&) = delete;
	static DescriptorManager* instance;

	struct SgrDescriptorInfo {
		std::string name;

		std::vector<VkVertexInputBindingDescription> vertexBindingDescr;
		std::vector<VkVertexInputAttributeDescription> vertexAttributeDescr;

		std::vector<VkDescriptorSetLayoutBinding> setLayoutBinding;
		std::vector<VkDescriptorSetLayout> setLayouts;
		VkDescriptorPool descriptorPool;
		std::vector<VkDescriptorSet> descriptorSets;
	};

	std::vector<SgrDescriptorInfo> descriptorInfos;

	SgrErrCode addNewDescriptorInfo(SgrDescriptorInfo& descrInfo);
	SgrErrCode updateDescriptorSets(std::string name, std::vector<void*> data);

	SgrDescriptorInfo getDescriptorInfoByName(std::string name);

public:
	static DescriptorManager* get();

protected:
	SgrErrCode createDescriptorSetLayout(SgrDescriptorInfo& descrInfo);
	SgrErrCode createDescriptorPool(SgrDescriptorInfo& descrInfo);
	SgrErrCode createDescriptorSets(SgrDescriptorInfo& descrInfo);
	std::vector<std::vector<VkWriteDescriptorSet>> createDescriptorSetWrites(SgrDescriptorInfo& descrInfo);
};