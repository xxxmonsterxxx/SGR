#include "PhysicalDeviceManager.h"

PhysicalDeviceManager::PhysicalDeviceManager()
{
}

PhysicalDeviceManager::~PhysicalDeviceManager()
{
}

sgrErrCode PhysicalDeviceManager::init(VkInstance instance)
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        return sgrGPUNotFound;
    }

    physicalDevices.resize(deviceCount);
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    printf("\nFinded devices:");
    for (auto device : devices) {
        VkPhysicalDeviceProperties deviceProp;
        vkGetPhysicalDeviceProperties(device, &deviceProp);
        printf("\n%s", deviceProp.deviceName);
        sgrPhysicalDevice newDeviceWithProp;
        newDeviceWithProp.first = device;
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        newDeviceWithProp.second.resize(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, newDeviceWithProp.second.data());

        physicalDevices.push_back(newDeviceWithProp);
    }

    return sgrOK;
}

sgrErrCode PhysicalDeviceManager::getPhysicalDeviceRequired(std::vector<VkQueueFlagBits> requiredQueues, sgrPhysicalDevice& device)
{
    for (auto physDev : physicalDevices) {
        uint8_t supportedQueues = 0;
        for (uint8_t i = 0; i < physDev.second.size(); i++) {
            for (auto reqQueue : requiredQueues) {
                if (physDev.second[i].queueFlags & reqQueue)
                    supportedQueues++;
            }
        }
        if (supportedQueues == requiredQueues.size()) {
            device = physDev;
            return sgrOK;
        } else {
            supportedQueues = 0;
        }
    }
    return sgrGPUNotFound;
}

