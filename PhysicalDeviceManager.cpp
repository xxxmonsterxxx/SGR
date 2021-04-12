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
    vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());

    return sgrOK;
}

std::vector<VkPhysicalDevice> PhysicalDeviceManager::getPhysDevInstances()
{
    return physicalDevices;
}

sgrErrCode PhysicalDeviceManager::setRenderPhysicalDevice(VkPhysicalDevice device)
{
    if (std::find(physicalDevices.begin(), physicalDevices.end(), device) == physicalDevices.end())
        return sgrGPUNotFound;

    physicalDevice = device;
    return sgrOK;
}
