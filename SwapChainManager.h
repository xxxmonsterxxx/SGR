#pragma once

#include "utils.h"

class SGR;

struct SgrSwapChainDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

class SwapChainManager {
public:
	static SwapChainManager* get();
	static void destroy();

	SgrSwapChainDetails querySwapChainDetails(VkPhysicalDevice device);
	void setSwapChainDeviceProperties(VkPhysicalDevice device);

private:
	SwapChainManager();
	~SwapChainManager();
	SwapChainManager(const SwapChainManager&) = delete;
	SwapChainManager& operator=(const SwapChainManager&) = delete;

	static SwapChainManager* instance;

	VkSurfaceKHR surface;

	sgrErrCode initSurface(VkInstance instance, GLFWwindow* window);

	SgrSwapChainDetails swapChainProps;

	friend class SGR;
};
