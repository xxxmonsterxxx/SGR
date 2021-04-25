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

VkShaderModule ShaderManager::createShader(const std::vector<char>& shaderCode)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = shaderCode.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(LogicalDeviceManager::get()->getLogicalDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

void ShaderManager::initShaders()
{
	FileManager* fm = FileManager::get();

    vertexModule = createShader(fm->readFile("SGRShaders/vertex.spv"));
    fragmentModule = createShader(fm->readFile("SGRShaders/fragment.spv"));
}

void ShaderManager::destroyShaders()
{
    vkDestroyShaderModule(LogicalDeviceManager::get()->getLogicalDevice(), fragmentModule, nullptr);
    vkDestroyShaderModule(LogicalDeviceManager::get()->getLogicalDevice(), vertexModule, nullptr);
}
