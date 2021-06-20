#pragma once

#include "Command.h"
#include "PipelineManager.h"

class BindDescriptorSetCommand : Command {

public:
	BindDescriptorSetCommand(VkPipelineLayout _pipelineLayout, VkDescriptorSet _descriptorSet, uint32_t _firstSet, uint32_t _descriptorSetCount, std::vector<uint32_t> _dynamicOffsets) :
		pipelineLayout(_pipelineLayout),
		descriptorSet(_descriptorSet),
		firstSet(_firstSet),
		descriptorSetCount(_descriptorSetCount),
		dynamicOffsets(_dynamicOffsets)
	{
		type = CommandType::BIND_DESCRIPTOR_SETS;
	}

private:
	VkPipelineLayout pipelineLayout;
	VkDescriptorSet descriptorSet;
	uint32_t firstSet;
	uint32_t descriptorSetCount;
	std::vector<uint32_t> dynamicOffsets;

	SgrErrCode execute(VkCommandBuffer* cmdBuffer) override {
		uint32_t* _dynamicOffsets = nullptr;
		if (dynamicOffsets.size() != 0)
			_dynamicOffsets = dynamicOffsets.data();
		vkCmdBindDescriptorSets(*cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, firstSet, descriptorSetCount, &descriptorSet, (uint32_t)dynamicOffsets.size(), _dynamicOffsets);
		return sgrOK;
	}
};