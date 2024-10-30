#pragma once

#include "utils.h"

class PipelineManager;
class SwapChainManager;
class CommandManager;
class SGR;
class UIManager;

class RenderPassManager {
	friend class PipelineManager;
	friend class SwapChainManager;
	friend class CommandManager;
	friend class SGR;
	friend class UIManager;
	
private:
	RenderPassManager();
	~RenderPassManager();
	RenderPassManager(const RenderPassManager&) = delete;
	RenderPassManager operator=(RenderPassManager&) = delete;
	static RenderPassManager* instance;

	VkRenderPass renderPass;

	SgrErrCode init();
	SgrErrCode destroyRenderPass();

	void destroy();
public:
	static RenderPassManager* get();
};
