#pragma once

#include "utils.h"
#include "SwapChainManager.h"

class SGR;

class TextureManager {
	friend class SGR;

public:
	static TextureManager* get();

	static SgrErrCode createTextureImage(std::string image_path, SgrImage*& image);

private:
	static TextureManager* instance;
	TextureManager();
	TextureManager(const TextureManager&) = delete;
	TextureManager& operator=(const TextureManager&) = delete;

protected:
	static SgrErrCode createTextureSampler(VkSampler& sampler);
};