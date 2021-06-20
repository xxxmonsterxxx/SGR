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

SgrErrCode MemoryManager::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags props, uint32_t& findedProps)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(PhysicalDeviceManager::instance->pickedPhysicalDevice.physDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & props) == props) {
            findedProps = i;
            return sgrOK;
        }
    }

    return sgrNoSuitableMemoryFinded;
}

SgrErrCode MemoryManager::createBuffer(SgrBuffer*& buffer, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
{
    SgrBuffer* newBuffer = new SgrBuffer;
    newBuffer->size = size;

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

    uint32_t memoryFindedIndex = 0;
    SgrErrCode resultSuitableMemoryIndex = findMemoryType(memRequirements.memoryTypeBits, properties, memoryFindedIndex);
    if (resultSuitableMemoryIndex != sgrOK)
        return resultSuitableMemoryIndex;

    allocInfo.memoryTypeIndex = memoryFindedIndex;

    if (vkAllocateMemory(device, &allocInfo, nullptr, &newBuffer->bufferMemory) != VK_SUCCESS) {
        return sgrAllocateMemoryError;
    }

    vkBindBufferMemory(device, newBuffer->vkBuffer, newBuffer->bufferMemory, 0);

    buffer = newBuffer;

    return sgrOK;
}

SgrErrCode MemoryManager::createStagingBufferWithData(SgrBuffer*& buffer, VkDeviceSize size, void* data)
{
    if (buffer != nullptr)
        return sgrIncorrectPointer;
    SgrErrCode resultInitStagingBuffer = createBuffer(buffer, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    if (resultInitStagingBuffer != sgrOK)
        return resultInitStagingBuffer;

    VkDevice device = LogicalDeviceManager::instance->logicalDevice;

    void* tempDataPointer;
    vkMapMemory(device, buffer->bufferMemory, 0, size, 0, &tempDataPointer);
    memcpy(tempDataPointer, data, (size_t)size);
    vkUnmapMemory(device, buffer->bufferMemory);

    return sgrOK;
}

SgrErrCode MemoryManager::createBufferUsingStaging(SgrBuffer*& buffer, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, void* data)
{
    SgrBuffer* stagingBuffer = nullptr;
    SgrErrCode resultInitStagingBuffer = createStagingBufferWithData(stagingBuffer, size, data);
    if (resultInitStagingBuffer != sgrOK)
        return resultInitStagingBuffer;

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
    VkCommandBuffer commandBuffer = CommandManager::instance->beginSingleTimeCommands();

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    CommandManager::instance->endSingleTimeCommands(commandBuffer);
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

void MemoryManager::copyBufferToImage(SgrBuffer* buffer, SgrImage* image) {
    VkCommandBuffer commandBuffer = CommandManager::instance->beginSingleTimeCommands();

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = {
        image->width,
        image->height,
        1
    };

    vkCmdCopyBufferToImage(commandBuffer, buffer->vkBuffer, image->vkImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    CommandManager::instance->endSingleTimeCommands(commandBuffer);
}
