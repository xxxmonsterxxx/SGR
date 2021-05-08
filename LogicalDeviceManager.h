#pragma once

#include "utils.h"

class SGR;
class SwapChainManager;

class LogicalDeviceManager {
	friend class SGR;
	friend class SwapChainManager;

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

	sgrErrCode initLogicalDevice();
};
