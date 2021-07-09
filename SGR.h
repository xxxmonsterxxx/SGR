#pragma once

// Simple Graphic Renderer (SGR) based on Vulkan library and GLFW.
// Mikhail Polestchuk, 2021, april.

#include "utils.h"

#include "WindowManager.h"
#include "PhysicalDeviceManager.h"
#include "SwapChainManager.h"
#include "LogicalDeviceManager.h"
#include "PipelineManager.h"
#include "CommandManager.h"
#include "MemoryManager.h"
#include "DescriptorManager.h"
#include "TextureManager.h"
#include "RenderPassManager.h"

class SGR {
public:
	struct SgrObject {
		std::string name;
		SgrBuffer* vertices;
		SgrBuffer* indices;
		bool meshDataAndPiplineBinded = false;
	};

	SgrBuffer* UBO;
	SgrBuffer* dynamicUBO;

	SGR(std::string appName = "Simple graphic application", uint8_t appVersionMajor = 1, uint8_t appVersionMinor = 0);
	~SGR(); 

	bool isSGRRunning();

	void (*drawDataUpdate)() = nullptr;
	void setUpdateFunction(void (*dataUpdateFunc)()) { drawDataUpdate = dataUpdateFunc; }

	SgrErrCode drawFrame();

	void setAspectRatio(uint8_t x, uint8_t y);

	/**
	 * Init SGR and create window automatically or with arguments.
	 * 
	 * \param windowWidth
	 * \param windowHeight
	 * \param windowName
	 * \return 
	 */
	SgrErrCode init(uint32_t windowWidth = 0, uint32_t windowHeight = 0, const char *windowName = "");

	SgrErrCode destroy();

	/**
	 * Init SGR Window manually. There will be possibility to init own special window from outside.
	 * 
	 * \param newWindow
	 * \param windowName
	 * \return 
	 */
	SgrErrCode initSGRWindow(GLFWwindow* newWindow, const char* windowName);

	std::vector<SgrPhysicalDevice> getAllPhysDevInstances();
	SgrErrCode setRenderPhysicalDevice(SgrPhysicalDevice device);

	void setRequiredQueueFamilies(std::vector<VkQueueFlagBits> reqFam);

	SgrErrCode addNewObjectGeometry(std::string name, std::vector<Sgr2DVertex> vertices, std::vector<uint16_t> indices,
									std::string shaderVert, std::string shaderFrag,
									std::vector<VkVertexInputBindingDescription> bindingDescriptions,
									std::vector<VkVertexInputAttributeDescription> attributDescrtions,
									std::vector<VkDescriptorSetLayoutBinding> setDescriptorSetsLayoutBinding);
	SgrErrCode writeDescriptorSets(std::string name, std::vector<void*> data);

	SgrErrCode setupUniformBuffers(SgrBuffer* uboBuffer, SgrBuffer* instanceUBO);
	SgrErrCode updateDynamicUniformBuffer(SgrDynamicUniformBufferObject dynamicUBO);
	SgrErrCode updateUniformBuffer(SgrUniformBufferObject obj);

	SgrErrCode drawObject(std::string objName, uint32_t dynamicUBOAlignment);
	void unbindAllMeshesAndPiplines();

private:
	const uint8_t engineVersionMajor = 0;
	const uint8_t engineVersionMinor = 1;
	const uint8_t enginePatch = 1;

	bool sgrRunning;
	SgrTime_t startRunning;

	std::string applicationName;
	uint8_t appVersionMajor;
	uint8_t appVersionMinor;

	WindowManager* windowManager;
	bool manualWindow;
	GLFWwindow* window = nullptr;

	VkInstance vulkanInstance;

	std::vector<VkQueueFlagBits> requiredQueueFamilies;
	std::vector<std::string> requiredExtensions;
	bool withSwapChain;
	PhysicalDeviceManager* physicalDeviceManager;
	LogicalDeviceManager* logicalDeviceManager;
	SwapChainManager* swapChainManager;
	PipelineManager* pipelineManager;
	CommandManager* commandManager;
	MemoryManager* memoryManager;
	DescriptorManager* descriptorManager;
	TextureManager* textureManager;
	RenderPassManager* renderPassManager;
	ShaderManager* shaderManager;

	uint8_t maxFrameInFlight;
	uint8_t currentFrame;

	uint32_t instanceUBOAlignment;

	std::vector<SgrObject> objects;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;

	SgrErrCode initSyncObjects();

	SgrErrCode initVulkanInstance();

	SgrObject& findObjectByName(std::string name);
};