#include "DescriptorManager.h"
#include "LogicalDeviceManager.h"
#include "SwapChainManager.h"

DescriptorManager* DescriptorManager::instance = nullptr;

DescriptorManager* DescriptorManager::get()
{
    if (instance == nullptr) {
        instance = new DescriptorManager();
        return instance;
    }
    else
        return instance;
}

DescriptorManager::DescriptorManager()
{
    defaultUBODescriptorPool = VK_NULL_HANDLE;
}

SgrErrCode DescriptorManager::initDefaultUBODescriptorSetLayouts()
{
    if (defaultUBODescriptorSetLayouts.size() != 0)
        return sgrOK;

    VkDescriptorSetLayoutBinding uboLayoutBinding;
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;

    VkDescriptorSetLayout descriptorSetLayout;
    if (vkCreateDescriptorSetLayout(LogicalDeviceManager::instance->logicalDevice, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
        return sgrInitDefaultUBODescriptorSetLayoutError;

    // if we dont specify different layout set for each swapchain image, then we'll use imageCount number simillar setLayouts
    for (size_t i = 0; i < SwapChainManager::instance->imageCount; i++)
        defaultUBODescriptorSetLayouts.push_back(descriptorSetLayout);

    return sgrOK;
}

SgrErrCode DescriptorManager::initDefaultUBODescriptorPool()
{
    if (defaultUBODescriptorPool != VK_NULL_HANDLE)
        return sgrOK;

    uint32_t swapChainImageCount = SwapChainManager::instance->imageCount;
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = swapChainImageCount;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = swapChainImageCount;

    if (vkCreateDescriptorPool(LogicalDeviceManager::instance->logicalDevice, &poolInfo, nullptr, &defaultUBODescriptorPool) != VK_SUCCESS)
        return sgrInitDefaultUBODescriptorPoolError;
    return sgrOK;
}

SgrErrCode DescriptorManager::initAndBindBufferUBODescriptors(std::vector<VkDescriptorSet>& descriptorSets,
                                                              std::vector<VkBuffer*> UBOBuffers,
                                                              VkDescriptorPool descriptorPool,
                                                              std::vector<VkDescriptorSetLayout> descriptorSetLayouts)
{
    if (descriptorSets.size() != UBOBuffers.size())
        return sgrWrongDescrAndUBOSize;

    if (descriptorPool == VK_NULL_HANDLE) {
        SgrErrCode resultInitDefaultUBODescriptorPool = initDefaultUBODescriptorPool();
        if (resultInitDefaultUBODescriptorPool != sgrOK)
            return resultInitDefaultUBODescriptorPool;
    }

    if (descriptorSetLayouts.size() == 0) {
        SgrErrCode resultInitDefaultDescriptorSetLayouts = initDefaultUBODescriptorSetLayouts();
        if (resultInitDefaultDescriptorSetLayouts != sgrOK)
            return resultInitDefaultDescriptorSetLayouts;
    }

    uint32_t swapChainImageCount = SwapChainManager::instance->imageCount;
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = defaultUBODescriptorPool;
    allocInfo.descriptorSetCount = swapChainImageCount;
    allocInfo.pSetLayouts = defaultUBODescriptorSetLayouts.data();
    descriptorSets.resize(swapChainImageCount);
    if (vkAllocateDescriptorSets(LogicalDeviceManager::instance->logicalDevice, &allocInfo, descriptorSets.data()) != VK_SUCCESS)
        return sgrInitDescriptorSetsError;

    for (size_t i = 0; i < descriptorSets.size(); i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = *UBOBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = descriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;
        descriptorWrite.pImageInfo = nullptr; // Optional
        descriptorWrite.pTexelBufferView = nullptr; // Optional

        vkUpdateDescriptorSets(LogicalDeviceManager::instance->logicalDevice, 1, &descriptorWrite, 0, nullptr);
    }

    return sgrOK;
}