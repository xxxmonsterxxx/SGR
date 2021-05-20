#pragma once

#include "utils.h"

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

	std::vector<VkDescriptorSetLayout> defaultUBODescriptorSetLayouts;
	SgrErrCode initDefaultUBODescriptorSetLayouts();

	VkDescriptorPool defaultUBODescriptorPool;
	SgrErrCode initDefaultUBODescriptorPool();

	SgrErrCode initAndBindBufferUBODescriptors(std::vector<VkDescriptorSet>& descriptorSets, std::vector<VkBuffer*> UBOBuffers,
								  VkDescriptorPool descriptorPool = VK_NULL_HANDLE,
								  std::vector<VkDescriptorSetLayout> descriptorSetLayouts = std::vector<VkDescriptorSetLayout>{});

public:
	static DescriptorManager* get();
};