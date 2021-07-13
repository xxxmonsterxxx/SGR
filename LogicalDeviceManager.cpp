#include "LogicalDeviceManager.h"
#include "PhysicalDeviceManager.h"

LogicalDeviceManager* LogicalDeviceManager::instance;

LogicalDeviceManager::LogicalDeviceManager() { ; }
LogicalDeviceManager::~LogicalDeviceManager() { ; }

LogicalDeviceManager* LogicalDeviceManager::get()
{
    if (instance == nullptr) {
        instance = new LogicalDeviceManager();
        return instance;
    }
    else {
        return instance;
    }
}

VkDevice LogicalDeviceManager::getLogicalDevice()
{
    return logicalDevice;
}

SgrErrCode LogicalDeviceManager::initLogicalDevice()
{
    // we need to create graphics and present queues
    PhysicalDeviceManager* physDeviceManager = PhysicalDeviceManager::get();
    SgrPhysicalDevice sgrDevice = physDeviceManager->getPickedPhysicalDevice();
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

    std::vector<const char*> enabledExtensions;
    uint32_t enabledExtensionsCount = static_cast<uint32_t>(physDeviceManager->getEnabledExtensions()->size());
    for (uint8_t i = 0; i < enabledExtensionsCount; i++) {
        enabledExtensions.push_back(physDeviceManager->getEnabledExtensions()->at(i).c_str());
    }

    createInfo.enabledExtensionCount = enabledExtensionsCount;
    createInfo.ppEnabledExtensionNames = enabledExtensions.data();
    createInfo.enabledLayerCount = 0;

    if (vkCreateDevice(sgrDevice.vkPhysDevice, &createInfo, nullptr, &logicalDevice) != VK_SUCCESS)
        return sgrInitLogicalDeviceError;

    vkGetDeviceQueue(logicalDevice, sgrDevice.fixedGraphicsQueue.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(logicalDevice, sgrDevice.fixedPresentQueue.value(), 0, &presentQueue);
    return sgrOK;
}

void LogicalDeviceManager::destroy()
{
    vkDestroyDevice(logicalDevice, nullptr);
    delete instance;
}
