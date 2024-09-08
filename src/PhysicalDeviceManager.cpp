#include "PhysicalDeviceManager.h"
#include "SwapChainManager.h"

PhysicalDeviceManager* PhysicalDeviceManager::instance = nullptr;

PhysicalDeviceManager::PhysicalDeviceManager() 
{
    pickedPhysicalDevice.vkPhysDevice = VK_NULL_HANDLE;
}

PhysicalDeviceManager::~PhysicalDeviceManager() { ; }

PhysicalDeviceManager* PhysicalDeviceManager::get()
{
    if (instance == nullptr) {
        instance = new PhysicalDeviceManager();
        return instance;
    }
    else {
        return instance;
    }
}

SgrErrCode PhysicalDeviceManager::init(VkInstance instance)
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        return sgrGPUNotFound;
    }

    std::vector<VkPhysicalDevice> devices;
    devices.resize(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    printf("\nFound %d devices:",deviceCount);
    for (auto device : devices) {
        VkPhysicalDeviceProperties deviceProp;
        vkGetPhysicalDeviceProperties(device, &deviceProp);
        printf("\n%s", deviceProp.deviceName);
        SgrPhysicalDevice newDeviceWithProp;
        newDeviceWithProp.vkPhysDevice = device;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        newDeviceWithProp.queueFamilies.resize(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, newDeviceWithProp.queueFamilies.data());

        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
        newDeviceWithProp.extensions.resize(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, newDeviceWithProp.extensions.data());

        vkGetPhysicalDeviceFeatures(device, &newDeviceWithProp.deviceFeatures);

        physicalDevices.push_back(newDeviceWithProp);
    }

    return sgrOK;
}

bool PhysicalDeviceManager::isSupportRequiredQueuesAndSurface(SgrPhysicalDevice& device, std::vector<VkQueueFlagBits> requiredQueues, VkSurfaceKHR* surface)
{
    uint8_t supportedQueues = 0;
    VkBool32 surfaceSupport = false;
    uint8_t queueIndex = 0;
    for (auto reqQueue : requiredQueues) {
        for (auto queueProp : device.queueFamilies) {
            if (queueProp.queueFlags & reqQueue) {
                supportedQueues++;
                if (!device.fixedGraphicsQueue.has_value() && surface != nullptr && reqQueue == VK_QUEUE_GRAPHICS_BIT) // if we need to check surface we should to save graphics queue index
                    device.fixedGraphicsQueue = queueIndex;
            }
            if (surface != nullptr && !device.fixedPresentQueue.has_value()) {
                vkGetPhysicalDeviceSurfaceSupportKHR(device.vkPhysDevice, queueIndex, *surface, &surfaceSupport);
                if (surfaceSupport)
                    device.fixedPresentQueue = queueIndex;
            }
            if ((queueProp.queueFlags & reqQueue) && device.fixedPresentQueue.has_value())
                break;
        }
    }

    if (supportedQueues == requiredQueues.size() && device.fixedPresentQueue.has_value())
        return true;

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
    SgrSwapChainDetails deviceSwapChainDetails = SwapChainManager::instance->querySwapChainDetails(sgrDevice.vkPhysDevice);
    if (!deviceSwapChainDetails.formats.empty() && !deviceSwapChainDetails.presentModes.empty())
        return true;

    return false;
}

bool PhysicalDeviceManager::isSupportSamplerAnisotropy(SgrPhysicalDevice sgrDevice)
{
    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(sgrDevice.vkPhysDevice, &supportedFeatures);
    if (supportedFeatures.samplerAnisotropy)
        return true;

    return false;
}

SgrErrCode PhysicalDeviceManager::findPhysicalDeviceRequired(std::vector<VkQueueFlagBits> requiredQueues,
                                                            std::vector<std::string> requiredExtensions,
                                                            VkSurfaceKHR surface)
{
    for (auto physDev : physicalDevices) {
        if (isSupportRequiredQueuesAndSurface(physDev, requiredQueues, &surface) &&
            isSupportRequiredExtentions(physDev, requiredExtensions) &&
            isSupportAnySwapChainMode(physDev)) {
                
                // if physical device support portability we MUST to add it to logical device extension
                std::vector<std::string> portabilityExtension; portabilityExtension.push_back("VK_KHR_portability_subset");
                if (isSupportRequiredExtentions(physDev, portabilityExtension))
                    requiredExtensions.push_back("VK_KHR_portability_subset");

                pickedPhysicalDevice = physDev;
                enabledExtensions = requiredExtensions;
                vkGetPhysicalDeviceProperties(pickedPhysicalDevice.vkPhysDevice, &pickedPhysicalDevice.props);
                return sgrOK;
        }
    }
    return sgrGPUNotFound;
}

SgrPhysicalDevice PhysicalDeviceManager::getPickedPhysicalDevice()
{
    return pickedPhysicalDevice;
}

std::vector<std::string>* PhysicalDeviceManager::getEnabledExtensions()
{
    return &enabledExtensions;
}

void PhysicalDeviceManager::destroy()
{
    enabledExtensions.clear();
    physicalDevices.clear();
    delete instance;
}
