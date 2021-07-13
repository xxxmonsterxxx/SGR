#pragma once

#include "utils.h"
#include "Command.h"
#include "PipelineManager.h"

class SGR;
class RenderPassManager;
class SwapChainManager;
class MemoryManager;
class TextureManager;

class CommandManager {
private:
	friend class SGR;
	friend class RenderPassManager;
	friend class SwapChainManager;
	friend class MemoryManager;
	friend class TextureManager;

	CommandManager();
	~CommandManager();
	CommandManager(const CommandManager&) = delete;
	CommandManager operator=(CommandManager&) = delete;

	static CommandManager* instance;

	static CommandManager* get();

	VkCommandPool commandPool = VK_NULL_HANDLE;
	SgrErrCode initCommandPool();

	std::vector<VkCommandBuffer> commandBuffers;
	SgrErrCode initCommandBuffers();
	SgrErrCode freeCommandBuffers();
	SgrErrCode endInitCommandBuffers();
	bool buffersEnded = false;

	std::vector<std::vector<Command*>> commands; // for each command buffer we have commands set
	SgrErrCode executeCommands();
	void addCmdToBuffer(int16_t bufferIndex, Command* newCmd);
	void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance, int16_t cmdBufferIndex = -1);
	void bindVertexBuffer(std::vector<VkBuffer> vertexBuffers, VkDeviceSize* offsets = nullptr, int16_t cmdBufferIndex = -1);
	void bindIndexBuffer(VkBuffer indexBuffer, int16_t cmdBufferIndex = -1);
	void drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance, int16_t cmdBufferIndex = -1);
	void bindDescriptorSet(VkPipelineLayout* pipelineLayout, uint8_t cmdBufferIndex, VkDescriptorSet descriptorSet, uint32_t firstSet, uint32_t descriptorSetCount, std::vector<uint32_t> dynamicOffsets = std::vector<uint32_t>{});
	void bindPipeline(VkPipeline* sgrPipeline, int16_t cmdBufferIndex = -1);

	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer cmdBuffer);

	void destroy();
};
