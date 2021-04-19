#include "PhysicalDeviceManager.h"

PhysicalDeviceManager::PhysicalDeviceManager()
{
    swapChainManager = SwapChainManager::get();
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
        SgrPhysicalDevice newDeviceWithProp;
        newDeviceWithProp.physDevice = device;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        newDeviceWithProp.queueFamilies.resize(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, newDeviceWithProp.queueFamilies.data());

        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
        newDeviceWithProp.extensions.resize(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, newDeviceWithProp.extensions.data());

        physicalDevices.push_back(newDeviceWithProp);
    }

    return sgrOK;
}

bool PhysicalDeviceManager::isSupportRequiredQueues(SgrPhysicalDevice device, std::vector<VkQueueFlagBits> requiredQueues)
{
    uint8_t supportedQueues = 0;
    for (auto reqQueue : requiredQueues) {
        for (auto queueProp : device.queueFamilies) {
            if (queueProp.queueFlags & reqQueue) {
                supportedQueues++;
                break;
            }
        }
    }

    if (supportedQueues == requiredQueues.size()) {
        return true;
    }

    return false;
}

bool PhysicalDeviceManager::isSupportRequiredExtentions(SgrPhysicalDevice device, std::vector<std::string> requiredExtensions)
{
    uint8_t supportedExtensions = 0;
    for (auto reqExt : requiredExtensions) {
        for (uint8_t i = 0; i < device.extensions.size(); i++) {
            if (strcmp(device.extensions[i].extensionName, reqExt.c_str())) {
                supportedExtensions++;
                break;
            }
        }
    }

    if (supportedExtensions == requiredExtensions.size())
        return true;

    return false;
}

bool PhysicalDeviceManager::isSupportAnySwapChainMode(SgrPhysicalDevice sgrDevice)
{
    SgrSwapChainDetails deviceSwapChainDetails = swapChainManager->querySwapChainDetails(sgrDevice.physDevice);

    if (!deviceSwapChainDetails.formats.empty() && !deviceSwapChainDetails.presentModes.empty())
        return true;

    return false;
}

sgrErrCode PhysicalDeviceManager::getPhysicalDeviceRequired(std::vector<VkQueueFlagBits> requiredQueues,
                                                            std::vector<std::string> requiredExtensions,
                                                            SgrPhysicalDevice& device,
                                                            bool withSwapChain)
{
    for (auto physDev : physicalDevices) {
        if (isSupportRequiredQueues(physDev, requiredQueues)) {
            if (isSupportRequiredExtentions(physDev, requiredExtensions)) {
                if (isSupportAnySwapChainMode(physDev)) {
                    device = physDev;
                    return sgrOK;
                }
            }
            if (!withSwapChain) {
                device = physDev;
                return sgrOK;
            }
        }
    }
    return sgrGPUNotFound;
}

