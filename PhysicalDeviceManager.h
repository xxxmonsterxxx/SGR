#pragma once

#include "utils.h"

class SGR;
class SwapChainManager;

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
	friend class SGR;
	friend class SwapChainManager;

	static PhysicalDeviceManager* instance;

	std::vector<SgrPhysicalDevice> physicalDevices;

	std::vector<std::string> enabledExtensions;
	SgrPhysicalDevice pickedPhysicalDevice;

	sgrErrCode init(VkInstance instance);

	bool isSupportRequiredQueuesAndSurface(SgrPhysicalDevice& sgrDevice, std::vector<VkQueueFlagBits> requiredQueues, VkSurfaceKHR* surface = nullptr);
	bool isSupportRequiredExtentions(SgrPhysicalDevice sgrDevice, std::vector<std::string> requiredExtensions);
	bool isSupportAnySwapChainMode(SgrPhysicalDevice sgrDevice);

	
	sgrErrCode findPhysicalDeviceRequired(std::vector<VkQueueFlagBits> requiredQueues,
		std::vector<std::string> requiredExtensions);

	sgrErrCode findPhysicalDeviceRequired(std::vector<VkQueueFlagBits> requiredQueues,
										 std::vector<std::string> requiredExtensions,
										 VkSurfaceKHR surface);

	PhysicalDeviceManager();
	~PhysicalDeviceManager();
	PhysicalDeviceManager(const PhysicalDeviceManager&) = delete;
	PhysicalDeviceManager& operator=(const PhysicalDeviceManager&) = delete;
public:
	
	static PhysicalDeviceManager* get();

	SgrPhysicalDevice getPickedPhysicalDevice();
	std::vector<std::string>* getEnabledExtensions();
};
