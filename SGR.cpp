#include "SGR.h"

SGR::SGR()
{
	manualWindow = false;
	sgrRunning = false;
	window = nullptr;
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
