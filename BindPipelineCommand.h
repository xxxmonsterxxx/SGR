#pragma once

#include "Command.h"

class BindPipelineCommand : Command {

public:
	BindPipelineCommand(VkPipeline _pipeline) :
		pipeline(_pipeline)
	{
		type = CommandType::BIND_INDEX_BUFFER;
	}

private:
	VkPipeline pipeline;

	SgrErrCode execute(VkCommandBuffer* cmdBuffer) override {
		vkCmdBindPipeline(*cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
		return sgrOK;
	}
};