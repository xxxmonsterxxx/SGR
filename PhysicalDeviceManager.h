#pragma once

#include "utils.h"

class SGR;

class PhysicalDeviceManager {
private:
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	std::vector<VkPhysicalDevice> physicalDevices;

	sgrErrCode init(VkInstance instance);

	friend class SGR;
public:
	PhysicalDeviceManager();
	~PhysicalDeviceManager();

	std::vector<VkPhysicalDevice> getPhysDevInstances();
	sgrErrCode setRenderPhysicalDevice(VkPhysicalDevice device);
};
