#pragma once

#include "utils.h"
#include "PhysicalDeviceManager.h"

class SGR;

class LogicalDeviceManager {
	friend class SGR;

public:
	LogicalDeviceManager();
	~LogicalDeviceManager();
private:
	VkDevice logicalDevice;

	VkQueue graphicsQueue;
	VkQueue presentQueue;

	sgrErrCode initLogicalDevice(SgrPhysicalDevice sgrDevice);
};
