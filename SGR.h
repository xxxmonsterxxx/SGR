#pragma once

// Simple Graphic Renderer (SGR) based on Vulkan library and GLFW.
// Mikhail Polestchuk, 2021, april.

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdint>
#include <string>

enum sgrErrCode
{
	sgrOK,
	sgrBadPointer
};

class SGR {
private:
	// default size of renderer's window
	uint32_t windowWidth;
	uint32_t windowHeight;
	std::string windowName;

	bool manualWindow;
	GLFWwindow* window; // render window. Now we have only one window to graphic render, but in future it could be some.

	/**
	 * Init SGR Window automatically.
	 * \param windowHeight
	 * \param windowWidth
	 * \param windowName
	 */
	void initSGRWindow();

	bool sgrRunning;

public:
	SGR();
	~SGR();

	bool isSGRRunning();
	sgrErrCode drawFrame();

	/**
	 * Init SGR and create window automatically or with arguments.
	 * 
	 * \param windowWidth
	 * \param windowHeight
	 * \param windowName
	 * \return 
	 */
	sgrErrCode init(uint32_t windowWidth = 0, uint32_t windowHeight = 0, const char *windowName = "");

	sgrErrCode destroy();

	/**
	 * Init SGR Window manually. There will be possibility to init own special window from outside.
	 * 
	 * \param newWindow
	 * \param windowName
	 * \return 
	 */
	sgrErrCode initSGRWindow(GLFWwindow* newWindow, const char* windowName);
};