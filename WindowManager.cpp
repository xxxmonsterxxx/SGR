#include "WindowManager.h"


static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
	auto app = reinterpret_cast<WindowManager*>(glfwGetWindowUserPointer(window));
	if (width == 0 || height == 0)
		app->windowMinimized = true;
	else {
		app->windowResized = true;
		app->windowMinimized = false;
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
	windowHeight = 600;
	windowName = "SGR";

	if (windowWidth != 0)
		this->windowWidth = windowWidth;
	if (windowHeight != 0)
		this->windowHeight = windowHeight;
	if (std::string(windowName).length() != 0)
		this->windowName = std::string(windowName);

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	window = glfwCreateWindow(this->windowWidth, this->windowHeight, this->windowName.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

	if (window == nullptr)
		return sgrInitWindowError;

	return sgrOK;
}

SgrErrCode WindowManager::init(GLFWwindow* newWindow, const char* windowName)
{
	if (newWindow == nullptr) {
		return sgrBadPointer;
	}

	glfwGetWindowSize(newWindow, (int*)(&windowWidth), (int*)&(windowHeight));
	this->windowName = std::string(windowName);
	window = newWindow;

	return sgrOK;
}

void WindowManager::destroy()
{
	glfwDestroyWindow(window);
}
