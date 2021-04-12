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
}

SGR::~SGR()
{
}

sgrErrCode SGR::init(uint32_t windowWidth, uint32_t windowHeight, const char *windowName)
{
	if (!manualWindow) {
		windowManager.init(windowWidth, windowHeight, windowName);
	}

	window = windowManager.window;
	if (window == nullptr)
		return sgrInitWindowError;

	sgrErrCode resultInitVulkan = initVulkanInstance();
	if (resultInitVulkan != sgrOK)
		return resultInitVulkan;

	if (physDeviceManager.init(vulkanInstance) != sgrOK)
		return sgrInitPhysicalDeviceManagerError;

	sgrRunning = true;

	return sgrOK;
}

sgrErrCode SGR::destroy()
{
	windowManager.destroy();
	glfwTerminate();

	return sgrOK;
}

sgrErrCode SGR::initSGRWindow(GLFWwindow* newWindow, const char* windowName)
{
	sgrErrCode resultCreateWindow = windowManager.init(newWindow, windowName);

	if (resultCreateWindow == sgrOK) {
		manualWindow = true;
		return resultCreateWindow;
	}
	
	return resultCreateWindow;
}

sgrErrCode SGR::drawFrame()
{
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
