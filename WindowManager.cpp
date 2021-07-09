#include "WindowManager.h"
#include "SGR.h"


void WindowManager::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
	auto app = reinterpret_cast<WindowManager*>(glfwGetWindowUserPointer(window));
	if (width == 0 || height == 0)
		app->windowMinimized = true;
	else {
		app->windowResized = true;
		app->windowMinimized = false;
		if (app->getParrentSGRptr()) {
			app->requestUpdateSwapChain();
			app->getParrentSGRptr()->drawFrame();
		}
	}
}

WindowManager* WindowManager::instance = nullptr;

WindowManager::WindowManager() { ; }
WindowManager::~WindowManager() { ; }

WindowManager* WindowManager::get()
{
	if (instance == nullptr) {
		instance = new WindowManager();
		return instance;
	}
	else {
		return instance;
	}
}

SgrErrCode WindowManager::init(uint32_t windowWidth, uint32_t windowHeight, const char* windowName)
{
	windowWidth = 800;
	windowHeight = 800;
	windowName = "SGR";

	if (windowWidth != 0)
		this->width = windowWidth;
	if (windowHeight != 0)
		this->height = windowHeight;
	if (std::string(windowName).length() != 0)
		this->name = std::string(windowName);

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	window = glfwCreateWindow(this->width, this->height, this->name.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	glfwSetWindowAspectRatio(window, 1, 1);

	if (window == nullptr)
		return sgrInitWindowError;

	return sgrOK;
}

SgrErrCode WindowManager::init(GLFWwindow* newWindow, const char* windowName)
{
	if (newWindow == nullptr) {
		return sgrBadPointer;
	}

	glfwGetWindowSize(newWindow, (int*)(&width), (int*)&(height));
	this->name = std::string(windowName);
	window = newWindow;

	return sgrOK;
}

void WindowManager::setAspectRatio(uint8_t x, uint8_t y)
{
	glfwSetWindowAspectRatio(window, x, y);
}

void WindowManager::requestUpdateSwapChain()
{
	SwapChainManager::instance->reinitSwapChain();
	windowResized = false;
}

void WindowManager::destroy()
{
	glfwDestroyWindow(window);
}
