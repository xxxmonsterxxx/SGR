#include "CommandManager.h"
#include "LogicalDeviceManager.h"
#include "SwapChainManager.h"
#include "RenderPassManager.h"
#include "PipelineManager.h"
#include "PhysicalDeviceManager.h"
#include "DrawCommand.h"
#include "BindVertexCommand.h"
#include "BindIndexCommand.h"
#include "DrawIndexedCommand.h"
#include "BindDescriptorSetCommand.h"
#include "BindPipelineCommand.h"
#include "UserInterface.h"

CommandManager* CommandManager::instance = nullptr;

CommandManager::CommandManager() { ; }
CommandManager::~CommandManager() { ; }

CommandManager* CommandManager::get()
{
    if (instance == nullptr) {
        instance = new CommandManager();
        return instance;
    }
    else
        return instance;
}

SgrErrCode CommandManager::initCommandPool()
{
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = PhysicalDeviceManager::get()->getPickedPhysicalDevice().fixedGraphicsQueue.value();
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(LogicalDeviceManager::instance->logicalDevice, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
        return sgrInitCommandPoolError;

    return sgrOK;
}

SgrErrCode CommandManager::beginCommandBuffers()
{
    for (size_t i = 0; i < commandBuffers.size(); i++) {
        // firstly we should to reset all command buffers
        if (vkResetCommandBuffer(commandBuffers[i], 0 /*VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT*/) != VK_SUCCESS)
            return sgrResetCommandBuffersError;

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0; // Optional
        beginInfo.pInheritanceInfo = nullptr; // Optional

        if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
            return sgrBeginCommandBufferError;

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = RenderPassManager::get()->renderPass;
        renderPassInfo.framebuffer = SwapChainManager::get()->framebuffers[i];
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = SwapChainManager::get()->extent;

        std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
		clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    return sgrOK;
}

SgrErrCode CommandManager::initCommandBuffers()
{
    if (commandPool == VK_NULL_HANDLE) {
        SgrErrCode resultInitCommandPool = initCommandPool();
        if (resultInitCommandPool != sgrOK)
            return resultInitCommandPool;
    }

    SwapChainManager* swpChMan = SwapChainManager::get();
    commandBuffers.resize(swpChMan->framebuffers.size());
    commands.resize(commandBuffers.size());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

    if (vkAllocateCommandBuffers(LogicalDeviceManager::instance->logicalDevice, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
        return sgrInitCommandBuffersError;

    return sgrOK;
}

SgrErrCode CommandManager::freeCommandBuffers(bool cleanOldCommands)
{
    vkFreeCommandBuffers(LogicalDeviceManager::instance->logicalDevice, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
    commandBuffers.clear();

    if (cleanOldCommands) {
        for (auto cmd : commands[0])
            delete cmd;
        commands.clear();
    }

    return sgrOK;
}

void CommandManager::addCmdToBuffer(int16_t bufferIndex, Command* newCmd)
{
    if (bufferIndex == -1) {
        for (size_t i = 0; i < commandBuffers.size(); i++)
            commands[i].push_back(newCmd);
    }
    else {
        commands[bufferIndex].push_back(newCmd);
    }
}

void CommandManager::draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance, int16_t cmdBufferIndex)
{
    DrawCommand* newDrawCmd = new DrawCommand(vertexCount, instanceCount, firstVertex, firstVertex);
    addCmdToBuffer(cmdBufferIndex, (Command*)newDrawCmd);
}

void CommandManager::bindVertexBuffer(std::vector<VkBuffer> vertexBuffers, VkDeviceSize* offsets, int16_t cmdBufferIndex)
{
    BindVertexCommand* newBindVertexCmd = new BindVertexCommand(vertexBuffers, offsets);
    addCmdToBuffer(cmdBufferIndex, (Command*)newBindVertexCmd);
}

void CommandManager::bindIndexBuffer(VkBuffer indexBuffer, int16_t cmdBufferIndex)
{
    BindIndexCommand* newBindIndexCmd = new BindIndexCommand(indexBuffer);
    addCmdToBuffer(cmdBufferIndex, (Command*)newBindIndexCmd);
}

void CommandManager::drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance, int16_t cmdBufferIndex)
{
    DrawIndexedCommand* newDrawIndexedCmd = new DrawIndexedCommand(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    addCmdToBuffer(cmdBufferIndex, (Command*)newDrawIndexedCmd);
}

void CommandManager::bindDescriptorSet(VkPipelineLayout* pipelineLayout, uint8_t cmdBufferIndex, VkDescriptorSet descriptorSet, uint32_t firstSet, uint32_t descriptorSetCount, std::vector<uint32_t> dynamicOffsets)
{
    BindDescriptorSetCommand* newBindDescriptorSetCmd = new BindDescriptorSetCommand(pipelineLayout, descriptorSet, firstSet, descriptorSetCount, dynamicOffsets);
    addCmdToBuffer(cmdBufferIndex, (Command*)newBindDescriptorSetCmd);
}

void CommandManager::bindPipeline(VkPipeline* sgrPipeline, int16_t cmdBufferIndex)
{
    BindPipelineCommand* newBindPipelineCmd = new BindPipelineCommand(sgrPipeline);
    addCmdToBuffer(cmdBufferIndex, (Command*)newBindPipelineCmd);
}

SgrErrCode CommandManager::endInitCommandBuffers()
{
    for (size_t i = 0; i < commandBuffers.size(); i++) {
        vkCmdEndRenderPass(commandBuffers[i]);

        if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
            return sgrEndCommandBufferError;
        }
    }

    return sgrOK;
}

SgrErrCode CommandManager::executeCommands()
{
    for (size_t i = 0; i < commandBuffers.size(); i++) {
        for (size_t j = 0; j < commands[i].size(); j++) {
            SgrErrCode resultCmd = commands[i][j]->execute(&commandBuffers[i]);
            if (resultCmd != sgrOK) {
                return resultCmd;
            }           
        }
    }

    return sgrOK;
}

VkCommandBuffer CommandManager::beginSingleTimeCommands()
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(LogicalDeviceManager::instance->logicalDevice, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void CommandManager::endSingleTimeCommands(VkCommandBuffer cmdBuffer)
{
    vkEndCommandBuffer(cmdBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuffer;

    VkQueue graphicsQueue = LogicalDeviceManager::instance->graphicsQueue;
    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);

    vkFreeCommandBuffers(LogicalDeviceManager::instance->logicalDevice, commandPool, 1, &cmdBuffer);
}

void CommandManager::destroy()
{
    VkDevice device = LogicalDeviceManager::instance->logicalDevice;
    for (auto cmd : commands[0])
        delete cmd;
    commands.clear();
    freeCommandBuffers();
    commandBuffers.clear();
    vkDestroyCommandPool(device, commandPool, nullptr);
    delete instance;
}
