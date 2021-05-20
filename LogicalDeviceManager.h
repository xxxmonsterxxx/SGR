#pragma once

#include "utils.h"

class SGR;
class SwapChainManager;
class MemoryManager;
class PipelineManager;
class DescriptorManager;

class LogicalDeviceManager {
	friend class SGR;
	friend class SwapChainManager;
	friend class MemoryManager;
	friend class PipelineManager;
	friend class DescriptorManager;

public:

	static LogicalDeviceManager* get();
	VkDevice getLogicalDevice();
	
private:
	LogicalDeviceManager();
	~LogicalDeviceManager();
	LogicalDeviceManager(const LogicalDeviceManager&) = delete;
	LogicalDeviceManager& operator=(const LogicalDeviceManager&) = delete;

	static LogicalDeviceManager* instance;

	VkDevice logicalDevice;

	VkQueue graphicsQueue;
	VkQueue presentQueue;

	SgrErrCode initLogicalDevice();
};
