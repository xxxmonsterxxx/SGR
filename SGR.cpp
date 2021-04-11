#include "SGR.h"

SGR::SGR()
{
	windowHeight = 600;
	windowWidth = 800;
	windowName = "SGR";
}

SGR::~SGR()
{

}

sgrErrCode SGR::init(uint32_t windowWidth, uint32_t windowHeight, const char *windowName)
{
	if (!manualWindow) {
		if (windowWidth != 0)
			this->windowWidth = windowWidth;
		if (windowHeight != 0)
			this->windowHeight = windowHeight;
		if (std::string(windowName).length() != 0)
			this->windowName = std::string(windowName);

		initSGRWindow();
	}

	sgrRunning = true;

	return sgrOK;
}

sgrErrCode SGR::destroy()
{
	glfwDestroyWindow(window);
	glfwTerminate();

	return sgrOK;
}

void SGR::initSGRWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(windowWidth, windowHeight, windowName.c_str(), nullptr, nullptr);
}

sgrErrCode SGR::initSGRWindow(GLFWwindow* newWindow, const char* windowName)
{
	manualWindow = true;
	if (newWindow == nullptr) {
		return sgrBadPointer;
	}

	glfwGetWindowSize(newWindow, (int*)(&windowWidth), (int*)&(windowHeight));
	this->windowName = std::string(windowName);
	window = newWindow;

	return sgrOK;
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
