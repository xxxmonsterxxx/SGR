#include "CommandManager.h"
#include "LogicalDeviceManager.h"
#include "SwapChainManager.h"
#include "RenderPassManager.h"
#include "PipelineManager.h"

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

sgrErrCode CommandManager::initCommandPool()
{
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = PhysicalDeviceManager::get()->getPickedPhysicalDevice().fixedGraphicsQueue.value();

    if (vkCreateCommandPool(LogicalDeviceManager::get()->getLogicalDevice(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
        return sgrInitCommandPoolError;

    return sgrOK;
}

sgrErrCode CommandManager::initCommandBuffers()
{
    sgrErrCode resultInitCommandPool = initCommandPool();
    if (resultInitCommandPool != sgrOK)
        return resultInitCommandPool;

    SwapChainManager* swpChMan = SwapChainManager::get();

    commandBuffers.resize(swpChMan->framebuffers.size());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

    if (vkAllocateCommandBuffers(LogicalDeviceManager::get()->getLogicalDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
        return sgrInitCommandBuffersError;

    for (size_t i = 0; i < commandBuffers.size(); i++) {
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

        VkClearValue clearColor = { 230.0f, 230.0f, 250.0f, 1.0f };
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, PipelineManager::get()->pipeline);
    }

    return sgrOK;
}

void CommandManager::draw(uint32_t commandBufferIndex, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{
    vkCmdDraw(commandBuffers[commandBufferIndex], vertexCount, instanceCount, firstVertex, firstInstance);
}

sgrErrCode CommandManager::endInitCommandBuffers()
{
    for (size_t i = 0; i < commandBuffers.size(); i++) {
        vkCmdEndRenderPass(commandBuffers[i]);

        if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
            return sgrEndCommandBufferError;
        }
    }

    buffersEnded = true;
    return sgrOK;
}
