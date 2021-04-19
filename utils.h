#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdint>
#include <string>
#include <vector>

#include <utility>

enum sgrErrCode
{
	sgrOK,
	sgrBadPointer,
	sgrInitWindowError,
	sgrGPUNotFound,
	sgrInitPhysicalDeviceManagerError,
	sgrInitVulkanError,
	sgrInitSurfaceError
};

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

struct SgrSwapChainDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};
