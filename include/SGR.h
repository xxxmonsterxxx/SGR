#pragma once

// Simple Graphic Renderer (SGR) based on Vulkan library and GLFW.
// Mikhail Polestchuk, 2021, april.

#include "version.h"
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
#include "UserInterface.h"

#define ON_SCREEN_RENDER true

#pragma pack(push, 1) // Disable padding
class SGR {
public:
	struct SgrObject {
		std::string name = "empty";
		SgrBuffer* vertices = nullptr;
		SgrBuffer* indices = nullptr;
		uint32_t indicesCount = 0;
		bool meshDataAndPiplineBinded = false;
	};

	struct SgrObjectInstance {
		std::string name = "empty";
		std::string geometry = "empty";
		uint32_t 	uboDataAlignment = 0;
		bool		needToDraw = false;
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

	SgrErrCode addNewObjectGeometry(std::string name, void* vertices, VkDeviceSize verticesSize, std::vector<uint32_t> indices,
									std::string shaderVert, std::string shaderFrag, bool filled,
									std::vector<VkVertexInputBindingDescription> bindingDescriptions,
									std::vector<VkVertexInputAttributeDescription> attributDescrtions,
									std::vector<VkDescriptorSetLayoutBinding> setDescriptorSetsLayoutBinding);

	SgrErrCode addObjectInstance(std::string name, std::string geometry, uint32_t dynamicUBOalignment);
	SgrErrCode writeDescriptorSets(std::string name, std::vector<void*> data);

	SgrErrCode setupGlobalUBO(SgrBuffer* uboBuffer);
	SgrErrCode updateGlobalUBO(SgrGlobalUBO obj);

	SgrErrCode updateInstancesUBO(SgrInstancesUBO& dynUBO);

	SgrErrCode drawObject(std::string instanceName);
	
	void unbindAllMeshesAndPiplines();

	SgrObjectInstance* findInstanceByName(std::string name);
	SgrObject* findObjectByName(std::string name);

	bool setFPSDesired(uint8_t fps);

	SgrErrCode getWindow(GLFWwindow* &ptr);
	SgrErrCode setApplicationLogo(std::string path);

	void enableDebugMode();

	SgrErrCode drawUIElement(SgrUIElement& uiElement);
	void setupUICallback();

private:
	const uint8_t engineVersionMajor = SGR_VERSION_MAJOR;
	const uint8_t engineVersionMinor = SGR_VERSION_MINOR;
	const uint8_t enginePatch 		 = SGR_VERSION_PATCH;

	bool sgrRunning;
	uint8_t fpsDesired = 60;

#if !NDBUG
	uint8_t fpsMin = 200;
	uint8_t fpsMax = 0;
#endif

	std::string applicationName;
	uint8_t appVersionMajor;
	uint8_t appVersionMinor;

	WindowManager* windowManager;
	bool manualWindow;
	GLFWwindow* window = nullptr;

	bool commandsBuilded = false;

	VkInstance vulkanInstance;

	std::vector<VkQueueFlagBits> requiredQueueFamilies;
	std::vector<std::string> instanceRequiredExtensions;
	std::vector<std::string> deviceRequiredExtensions;
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
	UIManager* uiManager;

	uint8_t maxFrameInFlight;
	uint8_t currentFrame;

	uint32_t instanceUBOAlignment;

	std::vector<SgrObject> objects;
	std::vector<SgrObjectInstance> instances;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;

	SgrErrCode initSyncObjects();

	SgrErrCode initVulkanInstance();

	SgrErrCode buildDrawingCommands(bool rebuild = false);

	// validation layer block
	const std::vector<const char*> requiredValidationLayers = {"VK_LAYER_KHRONOS_validation"};
	bool validationLayersEnabled = !NDBUG;
	VkDebugUtilsMessengerEXT debugMessenger;

	SgrErrCode checkValidationLayerSupport();
	void addGlfwRequiredExtensions();
	SgrErrCode checkRequiredExtensionsSupport();

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
														VkDebugUtilsMessageTypeFlagsEXT messageType,
														const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

	SgrErrCode destroyDebugMessenger();
};
#pragma pack(pop) // Enable padding