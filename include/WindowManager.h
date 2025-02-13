#pragma once

#include "utils.h"

class SGR;
class SwapChainManager;

class WindowManager {

private:
	// default size of renderer's window
	uint32_t width = 800;
	uint32_t height = 800;
	std::string name = "SGR";

	SGR* _parrentSgr = nullptr;

	float aspectRatioX = 1;
	float aspectRatioY = 1;
	void setAspectRatio(uint8_t x, uint8_t y);

	GLFWwindow* window; // render window. Now we have only one window to graphic render, but in future it could be some.

	/**
	 * Create window automatically or with arguments.
	 * 
	 * \param width
	 * \param height
	 * \param name
	 * \return 
	 */
	SgrErrCode init(uint32_t windowWidth, uint32_t windowHeight, const char* windowName);
	SgrErrCode setWindowIcons(GLFWimage* icons, uint8_t iconsNumber);

	/**
	 * Init Window manually. There will be possibility to init own special window from outside.
	 *
	 * \param newWindow
	 * \param name
	 * \return
	 */
	SgrErrCode init(GLFWwindow* newWindow, const char* windowName);
	void setSgrPtr(SGR* parrentSGR) { _parrentSgr = parrentSGR; }

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
	static void windowPosCallback(GLFWwindow* window, int xpos, int ypos);
	void requestUpdateSwapChain();

	void destroy();

	friend class SGR;
	friend class SwapChainManager;

	WindowManager();
	~WindowManager();
	WindowManager(const WindowManager&) = delete;
	WindowManager& operator=(const WindowManager&) = delete;

	static WindowManager* instance;
public:

	static WindowManager* get();
	bool resized = false;
	bool minimized = false;
	SGR* getParrentSGRptr() { return _parrentSgr; }
};