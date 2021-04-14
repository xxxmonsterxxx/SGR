#pragma once

#include "utils.h"

class SGR;

class PhysicalDeviceManager {
private:
	std::vector<sgrPhysicalDevice> physicalDevices;

	sgrErrCode init(VkInstance instance);

	sgrErrCode getPhysicalDeviceRequired(std::vector<VkQueueFlagBits> requiredQueues, sgrPhysicalDevice& device);

	friend class SGR;
public:
	PhysicalDeviceManager();
	~PhysicalDeviceManager();
};
