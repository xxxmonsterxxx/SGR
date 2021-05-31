#pragma once

#include "utils.h"
#include "SwapChainManager.h"

class SGR;

class TextureManager {
	friend class SGR;

public:
	static TextureManager* get();

private:
	static TextureManager* instance;
	TextureManager();
	TextureManager(const TextureManager&) = delete;
	TextureManager& operator=(const TextureManager&) = delete;

	SgrErrCode createTextureImage(std::string image_path, SgrImage*& image);
	SgrErrCode createTextureSampler(VkSampler* sampler);
};