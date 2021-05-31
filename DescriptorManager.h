#pragma once

#include "utils.h"
#include "SwapChainManager.h"

class SGR;
class PipelineManager;

class DescriptorManager {
	friend class SGR;
	friend class PipelineManager;

private:
	static DescriptorManager* instance;
	DescriptorManager();
	DescriptorManager(const DescriptorManager&) = delete;
	DescriptorManager& operator=(const DescriptorManager&) = delete;

	VkDescriptorPool defaultDescriptorPool;
	SgrErrCode initDefaultDescriptorPool();

	std::vector<VkDescriptorSetLayout> defaultDescriptorSetLayouts;
	SgrErrCode initDefaultDescriptorSetLayouts();

	SgrErrCode initAndBindDefaultDescriptors(std::vector<VkDescriptorSet>& descriptorSets, std::vector<VkBuffer*> UBOBuffers, std::vector<SgrImage*> textureImages);

	SgrErrCode initAndBindDescriptors(std::vector<VkDescriptorSet>& descriptorSets, std::vector<VkBuffer*> UBOBuffers,
								  VkDescriptorPool descriptorPool = VK_NULL_HANDLE,
								  std::vector<VkDescriptorSetLayout> descriptorSetLayouts = std::vector<VkDescriptorSetLayout>{});

	SgrErrCode bindDescriptors(std::vector<VkDescriptorSet> descriptorSets, std::vector<VkBuffer*> UBOBuffers,
		VkDescriptorPool descriptorPool = VK_NULL_HANDLE,
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts = std::vector<VkDescriptorSetLayout>{});

	SgrErrCode bindDescriptors(std::vector<std::vector<VkWriteDescriptorSet>> descriptorWrites);

public:
	static DescriptorManager* get();
};