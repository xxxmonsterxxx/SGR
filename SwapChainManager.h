#pragma once

#include "utils.h"

class SGR;
class PhysicalDeviceManager;
class CommandManager;
class DescriptorManager;
class TextureManager;

struct SgrSwapChainDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct SgrImage {
	VkImage vkImage;
	uint32_t width;
	uint32_t height;
	VkFormat format;
	VkImageUsageFlags usage;
	VkImageTiling tiling;
	VkMemoryPropertyFlags properties;
	VkDeviceMemory memory;
	VkImageView	view;
	VkSampler sampler;
};

class SwapChainManager {
	friend class SGR;
	friend class PhysicalDeviceManager;
	friend class CommandManager;
	friend class DescriptorManager;
	friend class TextureManager;

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

	static SgrErrCode createImageView(VkImage image, VkFormat format, VkImageView* imageView);
	static SgrErrCode createImage(SgrImage*& image);
	static SgrErrCode transitionImageLayout(SgrImage* image, VkImageLayout oldLayout, VkImageLayout newLayout);

	VkSwapchainKHR swapChain;
	SgrSwapChainDetails capabilities;
	uint32_t imageCount;
	std::vector<VkImage> images;
	VkFormat imageFormat;
	VkExtent2D extent;
	std::vector<VkImageView> imageViews;
	std::vector<VkFramebuffer> framebuffers;

	SgrErrCode initFrameBuffers();
};
