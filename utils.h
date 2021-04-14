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
	sgrInitVulkanError
};

class sgrPhysicalDevice : public std::pair<VkPhysicalDevice, std::vector<VkQueueFamilyProperties>> {
public:
	bool operator==(const sgrPhysicalDevice& comp) const
	{
		if (this->first == comp.first) {
			if (this->second.size() != comp.second.size())
				return false;

			for (uint8_t i = 0; i < this->second.size(); i++) {
				if (this->second[i].queueFlags != comp.second[i].queueFlags)
					return false;
			}
			return true;
		}
		return false;
	}
};
