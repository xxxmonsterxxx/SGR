#pragma once

#include "utils.h"
#include "SwapChainManager.h"

class SGR;

class PhysicalDeviceManager {
private:
	SwapChainManager* swapChainManager;

	std::vector<SgrPhysicalDevice> physicalDevices;

	sgrErrCode init(VkInstance instance);

	bool isSupportRequiredQueues(SgrPhysicalDevice sgrDevice, std::vector<VkQueueFlagBits> requiredQueues);
	bool isSupportRequiredExtentions(SgrPhysicalDevice sgrDevice, std::vector<std::string> requiredExtensions);
	bool isSupportAnySwapChainMode(SgrPhysicalDevice sgrDevice);

	sgrErrCode getPhysicalDeviceRequired(std::vector<VkQueueFlagBits> requiredQueues,
										 std::vector<std::string> requiredExtensions, SgrPhysicalDevice& sgrDevice, bool withSwapChain = true);

	friend class SGR;
public:
	PhysicalDeviceManager();
	~PhysicalDeviceManager();
};
