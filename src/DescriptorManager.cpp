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

SgrErrCode DescriptorManager::createDescriptorPool(SgrDescriptorInfo& descrInfo, VkDescriptorPool& descrPool)
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
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
 
    if (vkCreateDescriptorPool(LogicalDeviceManager::instance->logicalDevice, &poolInfo, nullptr, &descrPool) != VK_SUCCESS) {
        return sgrInitDefaultUBODescriptorPoolError;
	}

    return sgrOK;
}

SgrErrCode DescriptorManager::createDescriptorSets(std::string name, SgrDescriptorInfo& descrInfo)
{
	SgrDescriptorSets* newSets = new SgrDescriptorSets{};

    auto it = std::find_if(allDescriptorSets.begin(), allDescriptorSets.end(), [&name](const SgrDescriptorSets& descr){ return descr.name == name; });
    if (it != allDescriptorSets.end())
        newSets = &(*it);

	SgrErrCode resultCreateDescriptorPool = createDescriptorPool(descrInfo, newSets->descriptorPool);
	if (resultCreateDescriptorPool != sgrOK)
		return resultCreateDescriptorPool;

    uint32_t swapChainImageCount = SwapChainManager::instance->imageCount;
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = newSets->descriptorPool;
    allocInfo.descriptorSetCount = swapChainImageCount;
    allocInfo.pSetLayouts = descrInfo.setLayouts.data();
	newSets->descriptorSets.resize(swapChainImageCount);
    if (vkAllocateDescriptorSets(LogicalDeviceManager::instance->logicalDevice, &allocInfo, newSets->descriptorSets.data()) != VK_SUCCESS)
		return sgrInitDescriptorSetsError;

	newSets->name = name;

    if (it == allDescriptorSets.end())
	    allDescriptorSets.push_back(*newSets);
    return sgrOK;
}

SgrErrCode DescriptorManager::addNewDescriptorInfo(SgrDescriptorInfo& descrInfo)
{
    createDescriptorSetLayout(descrInfo);
    descriptorInfos.push_back(descrInfo);
    return sgrOK;
}

SgrErrCode DescriptorManager::updateDescriptorSets()
{
    VkDevice device = LogicalDeviceManager::instance->logicalDevice;
    int i = 0;
    for (auto& descr : pendedDescriptorsUpdate) {
        vkFreeDescriptorSets(device, allDescriptorSets[descr.idx].descriptorPool, allDescriptorSets[descr.idx].descriptorSets.size(), allDescriptorSets[descr.idx].descriptorSets.data());
        vkDestroyDescriptorPool(device, allDescriptorSets[descr.idx].descriptorPool, nullptr);

        if (updateDescriptorSets(descr.name, descr.infoName, descr.data, true) == sgrOK)
            i++;
    }

    if (i != pendedDescriptorsUpdate.size())
        return sgrDescrUpdateError;

    if (pendedDescriptorsUpdate.size() > 0) {
        pendedDescriptorsUpdate.clear();
        return sgrDescriptorsSetsUpdated;
    }
        
    return sgrOK;
}

SgrErrCode DescriptorManager::updateDescriptorSets(std::string name, std::string infoName, std::vector<void*> data, bool force)
{
	SgrDescriptorInfo info = getDescriptorInfoByName(infoName);
	if (info.name == "empty")
		return sgrDescriptorsWithUnknownInfo;

    size_t i = 0;
    for (; i <  allDescriptorSets.size(); ) {
        if (allDescriptorSets[i].name != name) {
            i++;
            continue;
        }
        else
            break;
    }

    if (i != allDescriptorSets.size() && !force) {
        SgrDescriptorPended descr{(int)i, name, infoName, data};
        pendedDescriptorsUpdate.push_back(descr);
        return sgrOK;
    } else {
		SgrErrCode resultCreateDescriptorSets = createDescriptorSets(name, info);
		if (resultCreateDescriptorSets != sgrOK) {
			return resultCreateDescriptorSets;
		}
	}

    std::vector<std::vector<VkWriteDescriptorSet>> descriptorWrites = createDescriptorSetWrites(getDescriptorSetsByName(name).descriptorSets, info);

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
                    dynamicUboBufferInfo.range = ((SgrBuffer*)data[k])->blockRange;

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

std::vector<std::vector<VkWriteDescriptorSet>> DescriptorManager::createDescriptorSetWrites(const std::vector<VkDescriptorSet>& descriptorSets, SgrDescriptorInfo& descrInfo)
{
    std::vector<std::vector<VkWriteDescriptorSet>> descriptorWrites;
    for (size_t i = 0; i < descriptorSets.size(); i++) {
        std::vector<VkWriteDescriptorSet> descriptorWritesForSet;
        for (size_t j = 0; j < descrInfo.setLayoutBinding.size(); j++) {
            VkWriteDescriptorSet newDescriptorWriteSet{};
            newDescriptorWriteSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            newDescriptorWriteSet.dstSet = descriptorSets[i];
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


const DescriptorManager::SgrDescriptorInfo DescriptorManager::getDescriptorInfoByName(std::string name)
{
    for (size_t i = 0; i < descriptorInfos.size(); i++) {
        if (descriptorInfos[i].name == name)
            return descriptorInfos[i];
    }

    SgrDescriptorInfo emptyDescriptorInfo;
    emptyDescriptorInfo.name = "empty";
    return emptyDescriptorInfo;
}

const DescriptorManager::SgrDescriptorSets DescriptorManager::getDescriptorSetsByName(std::string name)
{
	for (size_t i = 0; i < allDescriptorSets.size(); i++) {
        if (allDescriptorSets[i].name == name)
            return allDescriptorSets[i];
    }

    SgrDescriptorSets emptyDescriptorSets;
    emptyDescriptorSets.name = "empty";
    return emptyDescriptorSets;
}

SgrErrCode DescriptorManager::destroyDescriptorsData()
{
    VkDevice device = LogicalDeviceManager::instance->logicalDevice;

    for (auto& descrSets : allDescriptorSets)
        vkFreeDescriptorSets(device, descrSets.descriptorPool, descrSets.descriptorSets.size(), descrSets.descriptorSets.data());

    for (auto& descrSets : allDescriptorSets)
        vkDestroyDescriptorPool(device, descrSets.descriptorPool, nullptr);

    for (auto& descrInfo : descriptorInfos)
        vkDestroyDescriptorSetLayout(device, descrInfo.setLayouts[0], nullptr);

    vkDestroyDescriptorPool(device, uiDescriptorPool, nullptr);

    return sgrOK;
}

SgrErrCode DescriptorManager::createDescriptorPoolForUI()
{
    VkDescriptorPoolSize pool_sizes[] =
	{
		{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
	};

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	pool_info.maxSets = 1000;
	pool_info.poolSizeCount = std::size(pool_sizes);
	pool_info.pPoolSizes = pool_sizes;

	if(vkCreateDescriptorPool(LogicalDeviceManager::instance->logicalDevice, &pool_info, nullptr, &uiDescriptorPool) != VK_SUCCESS)
        return sgrDescriptorPoolCreateError;

    return sgrOK;
}