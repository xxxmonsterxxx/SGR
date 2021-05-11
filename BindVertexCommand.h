#pragma once

#include "Command.h"

class BindVertexCommand : Command {

public:
	BindVertexCommand(std::vector<VkBuffer> _vertexBuffers, VkDeviceSize* _offsets) :
		vertexBuffers(_vertexBuffers)
	{
		if (_offsets == nullptr) {
			offsets = new VkDeviceSize[0];
			offsets[0] = 0;
		}
		else {
			offsets = _offsets;
		}
		type = Command::CommandType::BIND_VERTEX_BUFFER;
	}

private:
	std::vector<VkBuffer> vertexBuffers;
	VkDeviceSize* offsets;

	SgrErrCode execute(VkCommandBuffer* cmdBuffer) override {
		vkCmdBindVertexBuffers(*cmdBuffer, 0, 1, vertexBuffers.data(), offsets);
		return sgrOK;
	}
};
