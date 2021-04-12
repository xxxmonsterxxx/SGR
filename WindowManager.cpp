#include "WindowManager.h"

WindowManager::WindowManager()
{
	windowWidth = 800;
	windowHeight = 600;
	windowName = "SGR";
}

WindowManager::~WindowManager()
{

}

sgrErrCode WindowManager::init(uint32_t windowWidth, uint32_t windowHeight, const char* windowName)
{
	if (windowWidth != 0)
		this->windowWidth = windowWidth;
	if (windowHeight != 0)
		this->windowHeight = windowHeight;
	if (std::string(windowName).length() != 0)
		this->windowName = std::string(windowName);

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(this->windowWidth, this->windowHeight, this->windowName.c_str(), nullptr, nullptr);
	if (window == nullptr)
		return sgrInitWindowError;

	return sgrOK;
}

sgrErrCode WindowManager::init(GLFWwindow* newWindow, const char* windowName)
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
