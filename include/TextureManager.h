#pragma once

#include "utils.h"
#include "SwapChainManager.h"

class SGR;

class TextureManager {
	friend class SGR;

public:
	static TextureManager* get();

	static SgrErrCode createTextureImage(std::string image_path, SgrImage*& image);
	static SgrErrCode createFontTextureImage(void* fontPixels, const uint32_t fontWidth, const uint32_t fontHeight, SgrImage*& image);

	static SgrErrCode destroyAllSamplers();

private:
	static TextureManager* instance;
	TextureManager();
	TextureManager(const TextureManager&) = delete;
	TextureManager& operator=(const TextureManager&) = delete;

	static SgrErrCode createImage(void* pixels, const uint32_t width, const uint32_t height, VkFormat format , SgrImage*& image);

	static std::vector<VkSampler*> createdSamplers;

protected:
	static SgrErrCode createTextureSampler(VkSampler& sampler);
};