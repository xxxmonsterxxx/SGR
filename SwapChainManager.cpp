#include "SwapChainManager.h"
#include "WindowManager.h"
#include "PhysicalDeviceManager.h"
#include "RenderPassManager.h"
#include "LogicalDeviceManager.h"
#include "CommandManager.h"
#include "PipelineManager.h"

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

void SwapChainManager::setSwapChainDeviceCapabilities(VkPhysicalDevice device)
{
    capabilities = querySwapChainDetails(device);
}

void SwapChainManager::setupSwapChainProperties()
{
    setSwapChainDeviceCapabilities(PhysicalDeviceManager::instance->pickedPhysicalDevice.physDevice);

    // choose surface color space format
    bool propertyChoosed = false;
    for (const auto& availableFormat : capabilities.formats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            surfaceFormat = availableFormat;
            propertyChoosed = true;
        }
    }
    if (!propertyChoosed)
        surfaceFormat = capabilities.formats[0];


    // choose present mode format
    propertyChoosed = false;
    for (const auto& availablePresentMode : capabilities.presentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            presentMode = availablePresentMode;
            propertyChoosed = true;
        }
    }
    if (!propertyChoosed)
        presentMode = VK_PRESENT_MODE_FIFO_KHR;

    // choose extend
    VkSurfaceCapabilitiesKHR caps = capabilities.capabilities;
    if (caps.currentExtent.width != UINT32_MAX) {
        extent = caps.currentExtent;
    }
    else {
        int width, height;
        glfwGetFramebufferSize(WindowManager::get()->window, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::max(caps.minImageExtent.width, std::min(caps.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(caps.minImageExtent.height, std::min(caps.maxImageExtent.height, actualExtent.height));

        extent = actualExtent;
    }

    imageCount = caps.minImageCount + 1;
    if (caps.maxImageCount > 0 && imageCount > caps.maxImageCount) {
        imageCount = caps.maxImageCount;
    }
}
sgrErrCode SwapChainManager::createImageViews() {
    VkDevice logicalDevice = LogicalDeviceManager::get()->getLogicalDevice();

    imageViews.resize(images.size());

    for (size_t i = 0; i < images.size(); i++) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = images[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = imageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(logicalDevice, &createInfo, nullptr, &imageViews[i]) != VK_SUCCESS)
            return sgrInitImageViews;
    }

    return sgrOK;
}


sgrErrCode SwapChainManager::initSwapChain()
{
    setupSwapChainProperties();

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    
    SgrPhysicalDevice ourDevice = PhysicalDeviceManager::get()->getPickedPhysicalDevice();
    uint32_t queueFamilyIndices[] = { ourDevice.fixedGraphicsQueue.value(), ourDevice.fixedPresentQueue.value() };
    if ( ourDevice.fixedGraphicsQueue != ourDevice.fixedPresentQueue) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = capabilities.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    VkDevice logicalDevice = LogicalDeviceManager::get()->getLogicalDevice();
    if (vkCreateSwapchainKHR(logicalDevice, &createInfo, nullptr, &swapChain) != VK_SUCCESS)
        return sgrInitSwapChainError;

    vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, nullptr);
    images.resize(imageCount);
    vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, images.data());

    imageFormat = surfaceFormat.format;

    sgrErrCode resultInitImageViews = createImageViews();
    if (resultInitImageViews != sgrOK)
        return resultInitImageViews;

    return sgrOK;
}

sgrErrCode SwapChainManager::cleanOldSwapChain()
{
    VkDevice device = LogicalDeviceManager::instance->logicalDevice;
    for (size_t i = 0; i < framebuffers.size(); i++) {
        vkDestroyFramebuffer(device, framebuffers[i], nullptr);
    }

    vkFreeCommandBuffers(device, CommandManager::instance->commandPool, static_cast<uint32_t>(CommandManager::instance->commandBuffers.size()), CommandManager::instance->commandBuffers.data());

    vkDestroyPipeline(device, PipelineManager::instance->pipeline, nullptr);
    vkDestroyPipelineLayout(device, PipelineManager::instance->pipelineLayout, nullptr);
    vkDestroyRenderPass(device, RenderPassManager::instance->renderPass, nullptr);

    for (size_t i = 0; i < imageViews.size(); i++) {
        vkDestroyImageView(device, imageViews[i], nullptr);
    }

    vkDestroySwapchainKHR(device, swapChain, nullptr);
    return sgrOK;
}

sgrErrCode SwapChainManager::reinitSwapChain()
{
    vkDeviceWaitIdle(LogicalDeviceManager::instance->logicalDevice);

    sgrErrCode resultCleanOldSwapChain = cleanOldSwapChain();
    if (resultCleanOldSwapChain != sgrOK)
        return resultCleanOldSwapChain;

    if (initSwapChain() != sgrOK)
        return sgrReinitSwapChainError;

    if (PipelineManager::instance->init() != sgrOK)
        return sgrReinitPipelineError;

    if (initFrameBuffers() != sgrOK)
        return sgrReinitFrameBuffersError;

    if (CommandManager::instance->initCommandBuffers() != sgrOK)
        return sgrReinitCommandBuffersError;

    return sgrOK;
}

sgrErrCode SwapChainManager::initSurface(VkInstance instance, GLFWwindow* window)
{
	if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
		return sgrInitSurfaceError;
	}
	return sgrOK;
}

VkExtent2D SwapChainManager::getExtent()
{
    return extent;
}

VkFormat SwapChainManager::getImageFormat()
{
    return imageFormat;
}

sgrErrCode SwapChainManager::initFrameBuffers()
{
    VkDevice logicalDevice = LogicalDeviceManager::get()->getLogicalDevice();

    framebuffers.resize(imageViews.size());

    for (size_t i = 0; i < imageViews.size(); i++) {
        VkImageView attachments[] = {
            imageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = RenderPassManager::get()->renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = extent.width;
        framebufferInfo.height = extent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(logicalDevice, &framebufferInfo, nullptr, &framebuffers[i]) != VK_SUCCESS)
            return sgrInitFrameBuffersError;
    }

    return sgrOK;
}
