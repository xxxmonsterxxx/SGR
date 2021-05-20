#pragma once

#include "Command.h"
#include "PipelineManager.h"

class BindDescriptorSetCommand : Command {

public:
	BindDescriptorSetCommand(VkDescriptorSet _descriptorSet, uint32_t _firstSet, uint32_t _descriptorSetCount, std::vector<uint32_t> _dynamicOffsets) :
		descriptorSet(_descriptorSet),
		firstSet(_firstSet),
		descriptorSetCount(_descriptorSetCount),
		dynamicOffsets(_dynamicOffsets)
	{
		type = CommandType::BIND_DESCRIPTOR_SETS;
	}

private:
	VkDescriptorSet descriptorSet;
	uint32_t firstSet;
	uint32_t descriptorSetCount;
	std::vector<uint32_t> dynamicOffsets;

	SgrErrCode execute(VkCommandBuffer* cmdBuffer) override {
		uint32_t* _dynamicOffsets = nullptr;
		if (dynamicOffsets.size() != 0)
			_dynamicOffsets = dynamicOffsets.data();
		vkCmdBindDescriptorSets(*cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, PipelineManager::instance->pipelineLayout, firstSet, descriptorSetCount, &descriptorSet, (uint32_t)dynamicOffsets.size(), _dynamicOffsets);
		return sgrOK;
	}
};