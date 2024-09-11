#include "ShaderManager.h"
#include "FileManager.h"
#include "LogicalDeviceManager.h"

ShaderManager* ShaderManager::instance = nullptr;

ShaderManager::ShaderManager() { ; }
ShaderManager::~ShaderManager() { ; }

ShaderManager* ShaderManager::get()
{
	if (instance == nullptr) {
		instance = new ShaderManager();
		return instance;
	}
	else
		return instance;
}

VkShaderModule ShaderManager::createShader(std::string filePath)
{
    std::vector<char> shaderCode = FileManager::readFile(filePath);
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = shaderCode.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(LogicalDeviceManager::get()->getLogicalDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::printf("Failed to create shader module from path %s", filePath.c_str());
    }

    return shaderModule;
}

SgrErrCode ShaderManager::createShaders(std::string name, std::string vertexShaderPath, std::string fragmentShaderPath)
{
    VkShaderModule newVertexShader = createShader(vertexShaderPath);
    VkShaderModule newFragmentShader = createShader(fragmentShaderPath);
    SgrShader newShaders{ name,newVertexShader,newFragmentShader };
    objectShaders.push_back(newShaders);

    return sgrOK;
}

SgrErrCode ShaderManager::destroyShaders(std::string name)
{
    VkDevice device = LogicalDeviceManager::get()->getLogicalDevice();
    for (size_t i = 0; i < objectShaders.size(); i++) {
        if (objectShaders[i].name == name) {
            vkDestroyShaderModule(device, objectShaders[i].vkShaders.vertex, nullptr);
            vkDestroyShaderModule(device, objectShaders[i].vkShaders.fragment, nullptr);
            return sgrOK;
        }
    }
    return sgrShaderToDeleteNotFound;
}

SgrErrCode ShaderManager::destroyAllShaders()
{
    for (auto shader : objectShaders) {
        destroyShaders(shader.name);
    }

    objectShaders.clear();
    
    return sgrOK;
}

ShaderManager::SgrShader ShaderManager::getShadersByName(std::string name)
{
    for (auto shaders : objectShaders) {
        if (shaders.name == name)
            return shaders;
    }

    SgrShader emptyShaders;
    emptyShaders.name = "empty";
    return emptyShaders;
}

void ShaderManager::destroy()
{
    destroyAllShaders();
    delete instance;
}
