#pragma once

#include "utils.h"
#include "SwapChainManager.h"

class SGR;

struct SgrPhysicalDevice {
	VkPhysicalDevice physDevice;
	std::vector<VkQueueFamilyProperties> queueFamilies;
	std::vector<VkExtensionProperties> extensions;

	bool operator==(const SgrPhysicalDevice& comp) const
	{
		if (this->physDevice == comp.physDevice) {
			if (this->queueFamilies.size() != comp.queueFamilies.size())
				return false;
			if (this->extensions.size() != comp.extensions.size())
				return false;

			for (uint8_t i = 0; i < this->queueFamilies.size(); i++) {
				if (this->queueFamilies[i].queueFlags != comp.queueFamilies[i].queueFlags)
					return false;
			}

			for (uint8_t i = 0; i < this->extensions.size(); i++) {
				if (!strcmp(extensions[i].extensionName, comp.extensions[i].extensionName))
					return false;
			}
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
