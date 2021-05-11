#pragma once

#include "utils.h"

class CommandManager;

class Command {
	friend class CommandManager;

protected:

	enum CommandType {
		NONE,
		DRAW,
		BIND_VERTEX_BUFFER,
		BIND_INDEX_BUFFER,
		DRAW_INDEXED
	};

	virtual SgrErrCode execute(VkCommandBuffer* cmdBuffer) = 0;

	CommandType type = CommandType::NONE;
};
