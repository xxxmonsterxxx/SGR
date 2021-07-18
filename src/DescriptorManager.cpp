#include "DescriptorManager.h"
#include "LogicalDeviceManager.h"
#include "MemoryManager.h"

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

DescriptorManager::DescriptorManager() { ; }

SgrErrCode DescriptorManager::createDescriptorSetLayout(SgrDescriptorInfo& descrInfo)
{
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(descrInfo.setLayoutBinding.size());
    layoutInfo.pBindings = descrInfo.setLayoutBinding.data();

    VkDescriptorSetLayout newLayout;
    if (vkCreateDescriptorSetLayout(LogicalDeviceManager::instance->logicalDevice, &layoutInfo, nullptr, &newLayout) != VK_SUCCESS)
        return sgrInitDefaultUBODescriptorSetLayoutError;

    std::vector<VkDescriptorSetLayout> newSetLayouts(SwapChainManager::instance->imageCount, newLayout);
    descrInfo.setLayouts = newSetLayouts;

    return sgrOK;
}

SgrErrCode DescriptorManager::createDescriptorPool(SgrDescriptorInfo& descrInfo)
{
    uint32_t swapChainImageCount = SwapChainManager::instance->imageCount;
    std::vector<VkDescriptorPoolSize> poolSizes;

    for (auto binding : descrInfo.setLayoutBinding) {
        VkDescriptorPoolSize poolSize;
        poolSize.type = binding.descriptorType;
        poolSize.descriptorCount = swapChainImageCount;
        poolSizes.push_back(poolSize);
    }

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = swapChainImageCount;
 
    if (vkCreateDescriptorPool(LogicalDeviceManager::instance->logicalDevice, &poolInfo, nullptr, &descrInfo.descriptorPool) != VK_SUCCESS)
        return sgrInitDefaultUBODescriptorPoolError;

    return sgrOK;
}

SgrErrCode DescriptorManager::createDescriptorSets(SgrDescriptorInfo& descrInfo)
{
    uint32_t swapChainImageCount = SwapChainManager::instance->imageCount;
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descrInfo.descriptorPool;
    allocInfo.descriptorSetCount = swapChainImageCount;
    allocInfo.pSetLayouts = descrInfo.setLayouts.data();
    descrInfo.descriptorSets.resize(swapChainImageCount);
    if (vkAllocateDescriptorSets(LogicalDeviceManager::instance->logicalDevice, &allocInfo, descrInfo.descriptorSets.data()) != VK_SUCCESS)
        return sgrInitDescriptorSetsError;
    return sgrOK;
}

SgrErrCode DescriptorManager::addNewDescriptorInfo(SgrDescriptorInfo& descrInfo)
{
    createDescriptorSetLayout(descrInfo);
    createDescriptorPool(descrInfo);
    createDescriptorSets(descrInfo);

    descriptorInfos.push_back(descrInfo);
    return sgrOK;
}

SgrErrCode DescriptorManager::updateDescriptorSets(std::string name, std::vector<void*> data)
{
    size_t i = 0;
    for (; i < descriptorInfos.size(); ) {
        if (descriptorInfos[i].name != name) {
            i++;
            continue;
        }
        else
            break;
    }

    if (i == descriptorInfos.size() + 1)
        return sgrDescrUpdateError;

    std::vector<std::vector<VkWriteDescriptorSet>> descriptorWrites = createDescriptorSetWrites(descriptorInfos[i]);

    for (size_t j = 0; j < descriptorWrites.size(); j++) {

        VkDescriptorBufferInfo uboBufferInfo{};
        VkDescriptorImageInfo imageInfo{};
        VkDescriptorBufferInfo dynamicUboBufferInfo{};

        for (size_t k = 0; k < descriptorWrites[j].size(); k++) {
            VkWriteDescriptorSet& descriptorWriteForSetOneBinding = descriptorWrites[j][k];
            switch (descriptorWriteForSetOneBinding.descriptorType) {
                case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                {
                    uboBufferInfo.buffer = ((SgrBuffer*)data[k])->vkBuffer;
                    uboBufferInfo.offset = 0;
                    uboBufferInfo.range = ((SgrBuffer*)data[k])->size;

                    descriptorWriteForSetOneBinding.pBufferInfo = &uboBufferInfo;
                    break;
                }
                case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
                {   
                    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    imageInfo.imageView = ((SgrImage*)data[k])->view;
                    imageInfo.sampler = ((SgrImage*)data[k])->sampler;

                    descriptorWriteForSetOneBinding.pImageInfo = &imageInfo;
                    break;
                }
                case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
                {   
                    dynamicUboBufferInfo.buffer = ((SgrBuffer*)data[k])->vkBuffer;
                    dynamicUboBufferInfo.offset = 0;
                    dynamicUboBufferInfo.range = ((SgrBuffer*)data[k])->size;

                    descriptorWriteForSetOneBinding.pBufferInfo = &dynamicUboBufferInfo;
                    break;
                }
                default:
                    return sgrUnknownVkDescriptorType;
            }
        }
        vkUpdateDescriptorSets(LogicalDeviceManager::instance->logicalDevice, static_cast<uint32_t>(descriptorWrites[j].size()), descriptorWrites[j].data(), 0, nullptr);
    }

    return sgrOK;
}

std::vector<std::vector<VkWriteDescriptorSet>> DescriptorManager::createDescriptorSetWrites(SgrDescriptorInfo& descrInfo)
{
    std::vector<std::vector<VkWriteDescriptorSet>> descriptorWrites;
    for (size_t i = 0; i < descrInfo.descriptorSets.size(); i++) {
        std::vector<VkWriteDescriptorSet> descriptorWritesForSet;
        for (size_t j = 0; j < descrInfo.setLayoutBinding.size(); j++) {
            VkWriteDescriptorSet newDescriptorWriteSet{};
            newDescriptorWriteSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            newDescriptorWriteSet.dstSet = descrInfo.descriptorSets[i];
            newDescriptorWriteSet.dstArrayElement = 0;
            newDescriptorWriteSet.descriptorCount = descrInfo.setLayoutBinding[j].descriptorCount;
            newDescriptorWriteSet.dstBinding = descrInfo.setLayoutBinding[j].binding;
            newDescriptorWriteSet.descriptorType = descrInfo.setLayoutBinding[j].descriptorType;

            descriptorWritesForSet.push_back(newDescriptorWriteSet);
        }
        descriptorWrites.push_back(descriptorWritesForSet);
    }

    return descriptorWrites;
}


DescriptorManager::SgrDescriptorInfo DescriptorManager::getDescriptorInfoByName(std::string name)
{
    for (size_t i = 0; i < descriptorInfos.size(); i++) {
        if (descriptorInfos[i].name == name)
            return descriptorInfos[i];
    }

    SgrDescriptorInfo emptyDescriptorInfo;
    emptyDescriptorInfo.name = "empty";
    return emptyDescriptorInfo;
}
