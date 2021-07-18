#pragma once

#include "Command.h"

class BindIndexCommand : Command {

public:
	BindIndexCommand(VkBuffer _indexBuffer) :
		indexBuffer(_indexBuffer)
	{
		type = CommandType::BIND_INDEX_BUFFER;
	}

private:
	VkBuffer indexBuffer;

	SgrErrCode execute(VkCommandBuffer* cmdBuffer) override {
		vkCmdBindIndexBuffer(*cmdBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);
		return sgrOK;
	}
};
