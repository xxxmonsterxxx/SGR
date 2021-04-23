#pragma once

#include "utils.h"
#include "PhysicalDeviceManager.h"

class SGR;

class LogicalDeviceManager {
	friend class SGR;

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
