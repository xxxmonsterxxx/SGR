#include "MemoryManager.h"
#include "LogicalDeviceManager.h"
#include "PhysicalDeviceManager.h"
#include "CommandManager.h"

MemoryManager* MemoryManager::instance;

MemoryManager::MemoryManager() { ; }
MemoryManager::~MemoryManager() { ; }

MemoryManager* MemoryManager::get()
{
    if (instance == nullptr) {
        instance = new MemoryManager();
        return instance;
    }
    else {
        return instance;
    }
}

uint32_t MemoryManager::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags props)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(PhysicalDeviceManager::instance->pickedPhysicalDevice.physDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & props) == props) {
            return i;
        }
    }

    return uint32_t();
}

SgrErrCode MemoryManager::createBuffer(SgrBuffer*& buffer, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
{
    SgrBuffer* newBuffer = new SgrBuffer;

    VkDevice device = LogicalDeviceManager::instance->logicalDevice;

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &newBuffer->vkBuffer) != VK_SUCCESS)
        return sgrCreateVkBufferError;

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, newBuffer->vkBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &newBuffer->bufferMemory) != VK_SUCCESS) {
        return sgrAllocateMemoryError;
    }

    vkBindBufferMemory(device, newBuffer->vkBuffer, newBuffer->bufferMemory, 0);

    buffer = newBuffer;

    return sgrOK;
}

SgrErrCode MemoryManager::createBufferUsingStaging(SgrBuffer*& buffer, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, void* data)
{
    SgrBuffer* stagingBuffer = nullptr;
    SgrErrCode resultInitStagingBuffer = createBuffer(stagingBuffer, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    if (resultInitStagingBuffer != sgrOK)
        return resultInitStagingBuffer;

    VkDevice device = LogicalDeviceManager::instance->logicalDevice;
    void* tempDataPointer;
    vkMapMemory(device, stagingBuffer->bufferMemory, 0, size, 0, &tempDataPointer);
    memcpy(tempDataPointer, data, (size_t)size);
    vkUnmapMemory(device, stagingBuffer->bufferMemory);

    SgrErrCode resultInintVertexBuffer = createBuffer(buffer, size, usage, properties);
    if (resultInintVertexBuffer != sgrOK)
        return resultInintVertexBuffer;

    copyBuffer(stagingBuffer->vkBuffer, buffer->vkBuffer, size);
    destroyBuffer(stagingBuffer);
    return sgrOK;
}

void MemoryManager::destroyBuffer(SgrBuffer* buffer)
{
    VkDevice device = LogicalDeviceManager::instance->logicalDevice;
    vkDestroyBuffer(device, buffer->vkBuffer, nullptr);
    vkFreeMemory(device, buffer->bufferMemory, nullptr);
    delete buffer;
}

void MemoryManager::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    VkDevice device = LogicalDeviceManager::instance->logicalDevice;
    VkCommandPool commandPool = CommandManager::instance->commandPool;

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    VkQueue graphicsQueue = LogicalDeviceManager::instance->graphicsQueue;
    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);

    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

SgrErrCode MemoryManager::createVertexBuffer(SgrBuffer*& buffer, VkDeviceSize size, void* vertexData)
{
    if (buffer != nullptr)
        return sgrIncorrectPointer;
    SgrErrCode resultCreateBufferUsingStaging = createBufferUsingStaging(buffer, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertexData);
    if (resultCreateBufferUsingStaging != sgrOK)
        return resultCreateBufferUsingStaging;
    allocatedBuffers.push_back(buffer);
    return sgrOK;
}

SgrErrCode MemoryManager::createIndexBuffer(SgrBuffer*& buffer, VkDeviceSize size, void* indexData)
{
    if (buffer != nullptr)
        return sgrIncorrectPointer;
    SgrErrCode resultCreateBufferUsingStaging = createBufferUsingStaging(buffer, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexData);
    if (resultCreateBufferUsingStaging != sgrOK)
        return resultCreateBufferUsingStaging;
    allocatedBuffers.push_back(buffer);
    return sgrOK;
}

SgrErrCode MemoryManager::createUniformBuffer(SgrBuffer*& buffer, VkDeviceSize size)
{
    if (buffer != nullptr)
        return sgrIncorrectPointer;
    SgrErrCode resultCreateBuffer = createBuffer(buffer, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    if (resultCreateBuffer != sgrOK)
        return resultCreateBuffer;
    allocatedBuffers.push_back(buffer);
    return sgrOK;
}
