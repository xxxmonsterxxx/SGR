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
	};

	std::vector<SgrDescriptorInfo> descriptorInfos;

	struct SgrDescriptorSets {
		std::string name;
		VkDescriptorPool descriptorPool;
		std::vector<VkDescriptorSet> descriptorSets;
	};

	std::vector<SgrDescriptorSets> allDescriptorSets;

	SgrErrCode addNewDescriptorInfo(SgrDescriptorInfo& descrInfo);
	SgrErrCode updateDescriptorSets(std::string instanceName, std::string infoName, std::vector<void*> data);

	const SgrDescriptorInfo getDescriptorInfoByName(std::string name);
	const SgrDescriptorSets getDescriptorSetsByName(std::string name);

public:
	static DescriptorManager* get();

	SgrErrCode destroyDescriptorsData();

protected:
	SgrErrCode createDescriptorSetLayout(SgrDescriptorInfo& descrInfo);
	SgrErrCode createDescriptorPool(SgrDescriptorInfo& descrInfo, VkDescriptorPool& descrPool);
	SgrErrCode createDescriptorSets(std::string name, SgrDescriptorInfo& descrInfo);
	std::vector<std::vector<VkWriteDescriptorSet>> createDescriptorSetWrites(const std::vector<VkDescriptorSet>& descriptorSets, SgrDescriptorInfo& descrInfo);
};