#pragma once

#include "Command.h"

class DrawCommand : Command {
public:
	DrawCommand(uint32_t _vertexCount, uint32_t _instanceCount, uint32_t _firstVertex, uint32_t _firstInstance) :
		vertexCount(_vertexCount),
		instanceCount(_instanceCount),
		firstVertex(_firstVertex),
		firstInstance(_firstInstance)
	{
		type = CommandType::DRAW;
	}

	uint32_t vertexCount;
	uint32_t instanceCount;
	uint32_t firstVertex;
	uint32_t firstInstance;

	SgrErrCode execute(VkCommandBuffer* cmdBuffer) override {
		vkCmdDraw(*cmdBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
		return sgrOK;
	}
};
