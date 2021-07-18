#pragma once

#include "utils.h"

class PipelineManager;
class SGR;

class ShaderManager {
	friend class PipelineManager;
	friend class SGR;

private:
	ShaderManager();
	~ShaderManager();
	ShaderManager(const ShaderManager&) = delete;
	ShaderManager operator=(ShaderManager&) = delete;
	static ShaderManager* instance;

	struct SgrShaderPack {
		VkShaderModule vertex;
		VkShaderModule fragment;
	};

	struct SgrShader {
		std::string name;
		SgrShaderPack vkShaders;
	};

	std::vector<SgrShader> objectShaders;

	SgrErrCode createShaders(std::string name, std::string vertexShaderPath, std::string fragmentShaderPath);
	SgrErrCode destroyShaders(std::string name);
	SgrErrCode destroyAllShaders();
	SgrShader getShadersByName(std::string name);

	void destroy();

public:
	static ShaderManager* get();

protected:
	VkShaderModule createShader(std::string filePath);
};
