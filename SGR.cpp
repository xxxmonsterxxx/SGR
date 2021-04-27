#include "SGR.h"

SGR::SGR(std::string appName, uint8_t appVersionMajor, uint8_t appVersionMinor)
{
	manualWindow = false;
	sgrRunning = false;
	window = nullptr;
	applicationName = "Simple graphic application";
	appVersionMajor = 1;
	appVersionMinor = 0;
	applicationName = appName;
	this->appVersionMajor = appVersionMajor;
	this->appVersionMinor = appVersionMinor;
	requiredQueueFamilies.push_back(VK_QUEUE_GRAPHICS_BIT); // because graphics bit support also transfer bit
	requiredExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	withSwapChain = true;

	// right sequence of initialization
	windowManager = WindowManager::get();
	physicalDeviceManager = PhysicalDeviceManager::get();
	logicalDeviceManager = LogicalDeviceManager::get();
	swapChainManager = SwapChainManager::get();
	pipelineManager = PipelineManager::get();
	commandManager = CommandManager::get();
}

SGR::~SGR()
{
}

sgrErrCode SGR::init(uint32_t windowWidth, uint32_t windowHeight, const char *windowName)
{
	if (!manualWindow) {
		windowManager->init(windowWidth, windowHeight, windowName);
	}

	window = windowManager->window;
	if (window == nullptr)
		return sgrInitWindowError;

	sgrErrCode resultInitVulkan = initVulkanInstance();
	if (resultInitVulkan != sgrOK)
		return resultInitVulkan;

	sgrErrCode resultInitPhysicalDeviceManager = physicalDeviceManager->init(vulkanInstance);
	if (resultInitPhysicalDeviceManager != sgrOK)
		return resultInitPhysicalDeviceManager;

	sgrErrCode resultInitSurface = swapChainManager->initSurface(vulkanInstance, window);
	if (resultInitSurface != sgrOK)
		return resultInitSurface;

	sgrErrCode resultGetPhysicalDeviceRequired = physicalDeviceManager->findPhysicalDeviceRequired(requiredQueueFamilies, requiredExtensions, SwapChainManager::get()->surface);
	if (resultGetPhysicalDeviceRequired != sgrOK)
		return resultGetPhysicalDeviceRequired;

	sgrErrCode resultInitLogicalDevice = logicalDeviceManager->initLogicalDevice();
	if (resultInitLogicalDevice != sgrOK)
		return resultInitLogicalDevice;

	sgrErrCode resultInitSwapChain = swapChainManager->initSwapChain();
	if (resultInitSwapChain != sgrOK)
		return resultInitSwapChain;

	sgrErrCode resultInitPipeline = pipelineManager->init();
	if (resultInitPipeline != sgrOK)
		return resultInitPipeline;

	sgrErrCode resultInitFrameBuffers = swapChainManager->initFrameBuffers();
	if (resultInitFrameBuffers != sgrOK)
		return resultInitFrameBuffers;

	sgrErrCode resultInitCommandBuffers = commandManager->initCommandBuffers();
	if (resultInitCommandBuffers != sgrOK)
		return resultInitCommandBuffers;

	sgrRunning = true;

	return sgrOK;
}

sgrErrCode SGR::destroy()
{
	windowManager->destroy();
	glfwTerminate();

	return sgrOK;
}

sgrErrCode SGR::initSGRWindow(GLFWwindow* newWindow, const char* windowName)
{
	sgrErrCode resultCreateWindow = windowManager->init(newWindow, windowName);

	if (resultCreateWindow == sgrOK) {
		manualWindow = true;
		window = windowManager->window;
		return resultCreateWindow;
	}
	
	return resultCreateWindow;
}

sgrErrCode SGR::drawFrame()
{
	if (!commandManager->buffersEnded)
		commandManager->endInitCommandBuffers();

	return sgrOK;
}

bool SGR::isSGRRunning()
{
	glfwPollEvents();

	if (glfwWindowShouldClose(window))
		sgrRunning = false;

	return sgrRunning;
}

sgrErrCode SGR::initVulkanInstance()
{
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = applicationName.c_str();
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "Simple Graphic Renderer";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();
	createInfo.enabledLayerCount = 0;
	createInfo.pNext = nullptr;

	if (vkCreateInstance(&createInfo, nullptr, &vulkanInstance) != VK_SUCCESS) {
		return sgrInitVulkanError;
	}

	return sgrOK;
}

std::vector<SgrPhysicalDevice> SGR::getAllPhysDevInstances()
{
	return physicalDeviceManager->physicalDevices;
}

void SGR::setRequiredQueueFamilies(std::vector<VkQueueFlagBits> reqFam)
{
	requiredQueueFamilies = reqFam;
}

sgrErrCode SGR::drawSimpleTestObject()
{
	commandManager->draw(0, 3, 1, 0, 0);
	return sgrOK;
}

sgrErrCode SGR::setRenderPhysicalDevice(SgrPhysicalDevice sgrDevice)
{
	if (std::find(physicalDeviceManager->physicalDevices.begin(),
		physicalDeviceManager->physicalDevices.end(), sgrDevice) == physicalDeviceManager->physicalDevices.end())
		return sgrGPUNotFound;

	physicalDeviceManager->pickedPhysicalDevice = sgrDevice;
	return sgrOK;
}
