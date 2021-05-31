#include "DescriptorManager.h"
#include "LogicalDeviceManager.h"

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
    defaultDescriptorPool = VK_NULL_HANDLE;
}

SgrErrCode DescriptorManager::initDefaultDescriptorSetLayouts()
{
    if (defaultDescriptorSetLayouts.size() != 0)
        return sgrOK;

    VkDescriptorSetLayoutBinding uboLayoutBinding;
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = 1;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = bindings.size();
    layoutInfo.pBindings = bindings.data();

    VkDescriptorSetLayout descriptorSetLayout;
    if (vkCreateDescriptorSetLayout(LogicalDeviceManager::instance->logicalDevice, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
        return sgrInitDefaultUBODescriptorSetLayoutError;

    // if we dont specify different layout set for each swapchain image, then we'll use imageCount number simillar setLayouts
    for (size_t i = 0; i < SwapChainManager::instance->imageCount; i++)
        defaultDescriptorSetLayouts.push_back(descriptorSetLayout);

    return sgrOK;
}

SgrErrCode DescriptorManager::initDefaultDescriptorPool()
{
    if (defaultDescriptorPool != VK_NULL_HANDLE)
        return sgrOK;

    uint32_t swapChainImageCount = SwapChainManager::instance->imageCount;
    std::array<VkDescriptorPoolSize, 2> poolSizes{};

    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = swapChainImageCount;
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = swapChainImageCount;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = poolSizes.size();
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = swapChainImageCount;

    if (vkCreateDescriptorPool(LogicalDeviceManager::instance->logicalDevice, &poolInfo, nullptr, &defaultDescriptorPool) != VK_SUCCESS)
        return sgrInitDefaultUBODescriptorPoolError;
    return sgrOK;
}

SgrErrCode DescriptorManager::initAndBindDefaultDescriptors(std::vector<VkDescriptorSet>& descriptorSets, std::vector<VkBuffer*> UBOBuffers, std::vector<SgrImage*> textureImages)
{
    if (descriptorSets.size() != UBOBuffers.size())
        return sgrWrongDescrAndUBOSize;

    if (defaultDescriptorPool == VK_NULL_HANDLE) {
        SgrErrCode resultInitDefaultUBODescriptorPool = initDefaultDescriptorPool();
        if (resultInitDefaultUBODescriptorPool != sgrOK)
            return resultInitDefaultUBODescriptorPool;
    }

    if (defaultDescriptorSetLayouts.size() == 0) {
        SgrErrCode resultInitDefaultDescriptorSetLayouts = initDefaultDescriptorSetLayouts();
        if (resultInitDefaultDescriptorSetLayouts != sgrOK)
            return resultInitDefaultDescriptorSetLayouts;
    }

    uint32_t swapChainImageCount = SwapChainManager::instance->imageCount;
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = defaultDescriptorPool;
    allocInfo.descriptorSetCount = swapChainImageCount;
    allocInfo.pSetLayouts = defaultDescriptorSetLayouts.data();
    descriptorSets.resize(swapChainImageCount);
    if (vkAllocateDescriptorSets(LogicalDeviceManager::instance->logicalDevice, &allocInfo, descriptorSets.data()) != VK_SUCCESS)
        return sgrInitDescriptorSetsError;

    for (size_t i = 0; i < descriptorSets.size(); i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = *UBOBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = textureImages[i]->view;
        imageInfo.sampler = textureImages[i]->sampler;

        std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = descriptorSets[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = descriptorSets[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(LogicalDeviceManager::instance->logicalDevice, descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
    }

    return sgrOK;
}
