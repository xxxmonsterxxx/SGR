#pragma once

#include "Command.h"

class DrawIndexedCommand : Command {
public:
	DrawIndexedCommand(uint32_t _indexCount, uint32_t _instanceCount, uint32_t _firstIndex, int32_t _verteOffset, uint32_t _firstInstance) :
		indexCount(_indexCount),
		instanceCount(_instanceCount),
		firstIndex(_firstIndex),
		verteOffset(_verteOffset),
		firstInstance(_firstInstance)
	{
		type = CommandType::DRAW_INDEXED;
	}

	uint32_t indexCount;
	uint32_t instanceCount;
	uint32_t firstIndex;
	int32_t  verteOffset;
	uint32_t firstInstance;

	SgrErrCode execute(VkCommandBuffer* cmdBuffer) override {
		vkCmdDrawIndexed(*cmdBuffer, indexCount, instanceCount, firstIndex, verteOffset, firstInstance);
		return sgrOK;
	}
};
