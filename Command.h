#pragma once

class CommandManager;

class Command {
	friend class CommandManager;

protected:

	enum CommandType {
		NONE,
		DRAW
	};

	virtual sgrErrCode execute(VkCommandBuffer* cmdBuffer) = 0;

	CommandType type = CommandType::NONE;
};
