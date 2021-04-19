#include "SwapChainManager.h"

SwapChainManager* SwapChainManager::instance = nullptr;

SwapChainManager::SwapChainManager() { ; }
SwapChainManager::~SwapChainManager() { ; }

SwapChainManager* SwapChainManager::get()
{
    if (instance == nullptr) {
        instance = new SwapChainManager();
        return instance;
    } else {
        return instance;
    }
}

void SwapChainManager::destroy()
{
    delete instance;
}

SgrSwapChainDetails SwapChainManager::querySwapChainDetails(VkPhysicalDevice device)
{
    SgrSwapChainDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

void SwapChainManager::setSwapChainDeviceProperties(VkPhysicalDevice device)
{
    swapChainProps = querySwapChainDetails(device);
}

sgrErrCode SwapChainManager::initSurface(VkInstance instance, GLFWwindow* window)
{
	if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
		return sgrInitSurfaceError;
	}
	return sgrOK;
}
