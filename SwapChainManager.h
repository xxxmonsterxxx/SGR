#pragma once

#include "utils.h"

class SGR;
class PhysicalDeviceManager;
class CommandManager;

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
	VkExtent2D getExtent();
	VkFormat getImageFormat();

private:
	SwapChainManager();
	~SwapChainManager();
	SwapChainManager(const SwapChainManager&) = delete;
	SwapChainManager& operator=(const SwapChainManager&) = delete;

	static SwapChainManager* instance;

	void setSwapChainDeviceCapabilities(VkPhysicalDevice device);
	SgrErrCode initSwapChain();
	SgrErrCode cleanOldSwapChain();
	SgrErrCode reinitSwapChain();

	VkSurfaceKHR surface;
	VkSurfaceFormatKHR surfaceFormat;
	VkPresentModeKHR presentMode;

	SgrErrCode initSurface(VkInstance instance, GLFWwindow* window);
	SgrErrCode createImageViews();
	void setupSwapChainProperties();

	VkSwapchainKHR swapChain;
	SgrSwapChainDetails capabilities;
	uint32_t imageCount;
	std::vector<VkImage> images;
	VkFormat imageFormat;
	VkExtent2D extent;
	std::vector<VkImageView> imageViews;
	std::vector<VkFramebuffer> framebuffers;

	SgrErrCode initFrameBuffers();


	friend class SGR;
	friend class PhysicalDeviceManager;
	friend class CommandManager;
};
