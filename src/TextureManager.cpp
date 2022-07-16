#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "TextureManager.h"
#include "MemoryManager.h"
#include "SwapChainManager.h"
#include "CommandManager.h"
#include "LogicalDeviceManager.h"
#include "PhysicalDeviceManager.h"

TextureManager* TextureManager::instance = nullptr;

TextureManager::TextureManager() { ; }

TextureManager* TextureManager::get()
{
	if (instance == nullptr) {
		instance = new TextureManager();
		return instance;
	}

	return instance;
}

SgrErrCode TextureManager::createImage(void* pixels, const uint32_t width, const uint32_t height, VkFormat format , SgrImage*& image)
{
	if (!pixels)
        return sgrLoadImageError;

	uint8_t bytesInPixel;
	switch (format) {
		case VK_FORMAT_R8G8B8A8_SRGB:
			bytesInPixel = 4;
			break;
		case VK_FORMAT_R8_UNORM:
			bytesInPixel = 1;
			break;
		default:
			return sgrIncorrectImagePixelFormat;
	}

	VkDeviceSize imageSize = width * height * bytesInPixel;

	image = new SgrImage;

    SgrBuffer* stagingBuffer = nullptr;
    SgrErrCode resultInitStagingBufferWithData = MemoryManager::instance->createStagingBufferWithData(stagingBuffer, imageSize, pixels);
    if (resultInitStagingBufferWithData != sgrOK)
        return resultInitStagingBufferWithData;

    image->width = width;
    image->height = height;
    image->format = format;
    image->tiling = VK_IMAGE_TILING_OPTIMAL;
    image->usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    image->properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    SgrErrCode resultCreateImage = SwapChainManager::createImage(image);
    if (resultCreateImage != sgrOK)
        return resultCreateImage;

    SgrErrCode resultTransitionImageLayout = SwapChainManager::transitionImageLayout(image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    if (resultTransitionImageLayout != sgrOK)
        return resultTransitionImageLayout;

    MemoryManager::copyBufferToImage(stagingBuffer, image);

    resultTransitionImageLayout = SwapChainManager::transitionImageLayout(image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    if (resultTransitionImageLayout != sgrOK)
        return resultTransitionImageLayout;

    MemoryManager::destroyBuffer(stagingBuffer);

    SgrErrCode resultCreateImageView = SwapChainManager::createImageView(image->vkImage, image->format, VK_IMAGE_ASPECT_COLOR_BIT, &image->view);
    if (resultCreateImageView != sgrOK)
        return resultCreateImageView;

    SgrErrCode resultCreateSampler = createTextureSampler(image->sampler);
    if (resultCreateSampler != sgrOK)
        return resultCreateSampler;

	return sgrOK;
}

SgrErrCode TextureManager::createTextureImage(std::string image_path, SgrImage*& image)
{
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(image_path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	SgrErrCode result = createImage(pixels, texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, image);
    stbi_image_free(pixels);
	return result;
}

SgrErrCode TextureManager::createFontTextureImage(void* fontPixels, const uint32_t fontWidth, const uint32_t fontHeight, SgrImage*& image)
{
	return createImage(fontPixels, fontWidth, fontHeight, VK_FORMAT_R8_UNORM, image);
}

SgrErrCode TextureManager::createTextureSampler(VkSampler& sampler)
{
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = PhysicalDeviceManager::instance->pickedPhysicalDevice.props.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    if (vkCreateSampler(LogicalDeviceManager::instance->logicalDevice, &samplerInfo, nullptr, &sampler) != VK_SUCCESS)
        return sgrCreateSamplerError;

    return sgrOK;
}
