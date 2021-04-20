#include "LogicalDeviceManager.h"

LogicalDeviceManager::LogicalDeviceManager()
{
}

LogicalDeviceManager::~LogicalDeviceManager()
{
}

sgrErrCode LogicalDeviceManager::initLogicalDevice(SgrPhysicalDevice sgrDevice)
{
    // we need to create graphics and present queues
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    float queuePriority = 1.0f;
    std::set<uint8_t> uniquePhysicalDeviceQueueFamilies = { sgrDevice.fixedGraphicsQueue.value(),sgrDevice.fixedPresentQueue.value() };
    for (auto queueIndex : uniquePhysicalDeviceQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueIndex;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &sgrDevice.deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(sgrDevice.extensions.size());
    std::vector<const char*> supportedExtensionNames;
    for (auto extensionProp : sgrDevice.extensions)
        supportedExtensionNames.push_back(extensionProp.extensionName);
    createInfo.ppEnabledExtensionNames = supportedExtensionNames.data();
    createInfo.enabledLayerCount = 0;

    if (vkCreateDevice(sgrDevice.physDevice, &createInfo, nullptr, &logicalDevice) != VK_SUCCESS)
        return sgrInitLogicalDeviceError;

    vkGetDeviceQueue(logicalDevice, sgrDevice.fixedGraphicsQueue.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(logicalDevice, sgrDevice.fixedPresentQueue.value(), 0, &presentQueue);
    return sgrOK;
}
