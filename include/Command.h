#pragma once

#include "utils.h"

class CommandManager;

enum CommandType {
	NONE,
	DRAW,
	BIND_VERTEX_BUFFER,
	BIND_INDEX_BUFFER,
	DRAW_INDEXED,
	BIND_DESCRIPTOR_SETS,
	BIND_PIPELINE
};

class Command {
	friend class CommandManager;
public:
	CommandType getType() { return type; }
	virtual ~Command() { ; }
protected:

	virtual SgrErrCode execute(VkCommandBuffer* cmdBuffer) = 0;

	CommandType type = CommandType::NONE;
};
