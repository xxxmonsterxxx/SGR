#pragma once

#include "utils.h"

class SGR;
class PhysicalDeviceManager;
class CommandManager;
class DescriptorManager;
class TextureManager;
class WindowManager;

struct SgrSwapChainDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;

	void destroy() {
		this->formats.clear();
		this->presentModes.clear();
	}
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
	friend class WindowManager;

public:
	static SwapChainManager* get();

	SgrSwapChainDetails querySwapChainDetails(VkPhysicalDevice device);
	VkExtent2D getExtent();
	VkFormat getImageFormat();
	SgrErrCode getDepthFormat(VkFormat& depthFormat);

	SgrErrCode findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features, VkFormat& resFormat);

private:
	SwapChainManager();
	~SwapChainManager();
	SwapChainManager(const SwapChainManager&) = delete;
	SwapChainManager& operator=(const SwapChainManager&) = delete;

	static SwapChainManager* instance;

	void destroy(VkInstance instance);

	void setSwapChainDeviceCapabilities(VkPhysicalDevice device);
	SgrErrCode initSwapChain();
	SgrErrCode cleanOldSwapChain();
	SgrErrCode reinitSwapChain();

	VkSurfaceKHR surface;
	VkSurfaceFormatKHR surfaceFormat;
	VkPresentModeKHR presentMode;

	SgrErrCode initSurface(VkInstance instance, GLFWwindow* window);
	SgrErrCode createImageViews();
	SgrErrCode createDepthResources();
	void setupSwapChainProperties();

	static SgrErrCode createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView* imageView);
	static SgrErrCode createImage(SgrImage*& image);
	static SgrErrCode transitionImageLayout(SgrImage* image, VkImageLayout oldLayout, VkImageLayout newLayout);

	VkSwapchainKHR swapChain;
	SgrSwapChainDetails details;
	uint32_t imageCount;
	std::vector<VkImage> images;
	SgrImage* depthImage = nullptr;
	VkFormat imageFormat;
	VkExtent2D extent;
	std::vector<VkImageView> imageViews;
	std::vector<VkFramebuffer> framebuffers;

	SgrErrCode initFrameBuffers();
};
