#pragma once

#include "utils.h"

class SGR;
class SwapChainManager;
class MemoryManager;

class LogicalDeviceManager {
	friend class SGR;
	friend class SwapChainManager;
	friend class MemoryManager;

public:

	static LogicalDeviceManager* get();
	VkDevice getLogicalDevice();
	
private:
	LogicalDeviceManager();
	~LogicalDeviceManager();
	LogicalDeviceManager(const LogicalDeviceManager&) = delete;
	LogicalDeviceManager& operator=(const LogicalDeviceManager&) = delete;

	static LogicalDeviceManager* instance;

	VkDevice logicalDevice;

	VkQueue graphicsQueue;
	VkQueue presentQueue;

	SgrErrCode initLogicalDevice();
};
