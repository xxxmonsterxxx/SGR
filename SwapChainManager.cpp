#include "SwapChainManager.h"
#include "WindowManager.h"
#include "PhysicalDeviceManager.h"
#include "RenderPassManager.h"
#include "LogicalDeviceManager.h"
#include "CommandManager.h"
#include "PipelineManager.h"
#include "MemoryManager.h"

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

SgrErrCode SwapChainManager::createImageViews() {
    VkDevice logicalDevice = LogicalDeviceManager::get()->getLogicalDevice();

    imageViews.resize(images.size());

    SgrErrCode resultCreateImageView = sgrOK;
    for (size_t i = 0; i < images.size(); i++) {
        resultCreateImageView = createImageView(images[i], imageFormat, &imageViews[i]);
        if (resultCreateImageView != sgrOK)
            return sgrOK;
    }

    return sgrOK;
}


SgrErrCode SwapChainManager::initSwapChain()
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

    SgrErrCode resultInitImageViews = createImageViews();
    if (resultInitImageViews != sgrOK)
        return resultInitImageViews;

    return sgrOK;
}

SgrErrCode SwapChainManager::cleanOldSwapChain()
{
    VkDevice device = LogicalDeviceManager::instance->logicalDevice;
    for (size_t i = 0; i < framebuffers.size(); i++) {
        vkDestroyFramebuffer(device, framebuffers[i], nullptr);
    }

    vkFreeCommandBuffers(device, CommandManager::instance->commandPool, static_cast<uint32_t>(CommandManager::instance->commandBuffers.size()), CommandManager::instance->commandBuffers.data());

    PipelineManager::instance->destroyAllPipelines();
    vkDestroyRenderPass(device, RenderPassManager::instance->renderPass, nullptr);

    for (size_t i = 0; i < imageViews.size(); i++) {
        vkDestroyImageView(device, imageViews[i], nullptr);
    }

    vkDestroySwapchainKHR(device, swapChain, nullptr);
    return sgrOK;
}

SgrErrCode SwapChainManager::reinitSwapChain()
{
    vkDeviceWaitIdle(LogicalDeviceManager::instance->logicalDevice);

    SgrErrCode resultCleanOldSwapChain = cleanOldSwapChain();
    if (resultCleanOldSwapChain != sgrOK)
        return resultCleanOldSwapChain;

    if (initSwapChain() != sgrOK)
        return sgrReinitSwapChainError;

    if (PipelineManager::instance->reinitAllPipelines() != sgrOK)
        return sgrReinitPipelineError;

    if (initFrameBuffers() != sgrOK)
        return sgrReinitFrameBuffersError;

    if (CommandManager::instance->initCommandBuffers() != sgrOK)
        return sgrReinitCommandBuffersError;

    return sgrOK;
}

SgrErrCode SwapChainManager::initSurface(VkInstance instance, GLFWwindow* window)
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

SgrErrCode SwapChainManager::initFrameBuffers()
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

SgrErrCode SwapChainManager::createImage(SgrImage*& image)
{
    if (image == nullptr)
        return sgrIncorrectPointer;

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = image->width;
    imageInfo.extent.height = image->height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = image->format;
    imageInfo.tiling = image->tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = image->usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkDevice device = LogicalDeviceManager::instance->logicalDevice;
    if (vkCreateImage(device, &imageInfo, nullptr, &(image->vkImage)) != VK_SUCCESS)
        return sgrCreateImageError;

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, image->vkImage, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;

    uint32_t memoryFindedIndex = 0;
    SgrErrCode resultSuitableMemoryIndex = MemoryManager::findMemoryType(memRequirements.memoryTypeBits, image->properties, memoryFindedIndex);
    if (resultSuitableMemoryIndex != sgrOK)
        return resultSuitableMemoryIndex;
    allocInfo.memoryTypeIndex = memoryFindedIndex;

    if (vkAllocateMemory(device, &allocInfo, nullptr, &image->memory) != VK_SUCCESS)
        sgrAllocateMemoryError;

    vkBindImageMemory(device, image->vkImage, image->memory, 0);
    return sgrOK;
}

SgrErrCode SwapChainManager::transitionImageLayout(SgrImage* image, VkImageLayout oldLayout, VkImageLayout newLayout) {
    VkCommandBuffer commandBuffer = CommandManager::instance->beginSingleTimeCommands();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image->vkImage;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else {
        return sgrUnsupportedLayoutTransition;
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    CommandManager::instance->endSingleTimeCommands(commandBuffer);

    return sgrOK;
}


SgrErrCode SwapChainManager::createImageView(VkImage image, VkFormat format, VkImageView* imageView)
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(LogicalDeviceManager::instance->logicalDevice, &viewInfo, nullptr, imageView) != VK_SUCCESS)
        return sgrInitImageViews;

    return sgrOK;
}