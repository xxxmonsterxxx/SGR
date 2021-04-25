#pragma once

#include "utils.h"

class PipelineManager;
class SGR;

class ShaderManager {
private:
	ShaderManager();
	~ShaderManager();
	ShaderManager(const ShaderManager&) = delete;
	ShaderManager operator=(ShaderManager&) = delete;

	static ShaderManager* instance;

	VkShaderModule vertexModule;
	VkShaderModule fragmentModule;

	void initShaders();
	void destroyShaders();
	VkShaderModule createShader(const std::vector<char>& shaderCode);

	friend class PipelineManager;

public:
	static ShaderManager* get();
};
