#pragma once

#include "utils.h"

class SGR;
class SwapChainManager;
class MemoryManager;
class TextureManager;

struct SgrPhysicalDevice {
	VkPhysicalDevice vkPhysDevice;
	std::vector<VkQueueFamilyProperties> queueFamilies;
	std::vector<VkExtensionProperties> extensions;
	VkPhysicalDeviceFeatures deviceFeatures{};
	std::optional<uint8_t> fixedGraphicsQueue; // fixed index of queue with graphics support
	std::optional<uint8_t> fixedPresentQueue; // fixed index of queue with present support
	VkPhysicalDeviceProperties props;

	bool operator==(const SgrPhysicalDevice& comp) const
	{
		if (this->vkPhysDevice == comp.vkPhysDevice) {
			return true;
		}
		return false;
	}
};

class PhysicalDeviceManager {
private:
	friend class SGR;
	friend class SwapChainManager;
	friend class MemoryManager;
	friend class TextureManager;

	static PhysicalDeviceManager* instance;

	std::vector<SgrPhysicalDevice> physicalDevices;

	std::vector<std::string> enabledExtensions;
	SgrPhysicalDevice pickedPhysicalDevice;

	SgrErrCode init(VkInstance instance);

	bool isSupportRequiredQueuesAndSurface(SgrPhysicalDevice& sgrDevice, std::vector<VkQueueFlagBits> requiredQueues, VkSurfaceKHR* surface = nullptr);
	bool isSupportRequiredExtentions(SgrPhysicalDevice sgrDevice, std::vector<std::string> requiredExtensions);
	bool isSupportAnySwapChainMode(SgrPhysicalDevice sgrDevice);
	bool isSupportSamplerAnisotropy(SgrPhysicalDevice sgrDevice);

	
	SgrErrCode findPhysicalDeviceRequired(std::vector<VkQueueFlagBits> requiredQueues,
		std::vector<std::string> requiredExtensions);

	SgrErrCode findPhysicalDeviceRequired(std::vector<VkQueueFlagBits> requiredQueues,
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
