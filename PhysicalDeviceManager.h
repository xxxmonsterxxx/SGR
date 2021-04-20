#pragma once

#include "utils.h"
#include "SwapChainManager.h"

class SGR;

struct SgrPhysicalDevice {
	VkPhysicalDevice physDevice;
	std::vector<VkQueueFamilyProperties> queueFamilies;
	std::vector<VkExtensionProperties> extensions;
	VkPhysicalDeviceFeatures deviceFeatures{};
	std::optional<uint8_t> fixedGraphicsQueue; // fixed index of queue with graphics support
	std::optional<uint8_t> fixedPresentQueue; // fixed index of queue with present support

	bool operator==(const SgrPhysicalDevice& comp) const
	{
		if (this->physDevice == comp.physDevice) {
			return true;
		}
		return false;
	}
};

class PhysicalDeviceManager {
private:
	SwapChainManager* swapChainManager;

	std::vector<SgrPhysicalDevice> physicalDevices;

	sgrErrCode init(VkInstance instance);

	bool isSupportRequiredQueuesAndSurface(SgrPhysicalDevice& sgrDevice, std::vector<VkQueueFlagBits> requiredQueues, VkSurfaceKHR* surface = nullptr);
	bool isSupportRequiredExtentions(SgrPhysicalDevice sgrDevice, std::vector<std::string> requiredExtensions);
	bool isSupportAnySwapChainMode(SgrPhysicalDevice sgrDevice);

	
	sgrErrCode getPhysicalDeviceRequired(std::vector<VkQueueFlagBits> requiredQueues,
		std::vector<std::string> requiredExtensions,
		SgrPhysicalDevice& sgrDevice);

	sgrErrCode getPhysicalDeviceRequired(std::vector<VkQueueFlagBits> requiredQueues,
										 std::vector<std::string> requiredExtensions,
										 VkSurfaceKHR surface,
										 SgrPhysicalDevice& sgrDevice);

	friend class SGR;
public:
	PhysicalDeviceManager();
	~PhysicalDeviceManager();
};
