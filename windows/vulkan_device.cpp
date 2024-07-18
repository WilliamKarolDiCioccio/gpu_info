#include <iostream>
#include <set>
#include <sstream>

#include "vulkan_device.hpp"

namespace gpu_info
{

VulkanQueueFamilySupportDetails FindVulkanDeviceQueueFamiliesSupport(const VkPhysicalDevice &_device)
{
    VulkanQueueFamilySupportDetails indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(_device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(_device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto &queueFamily : queueFamilies)
    {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphicsFamily = i;
        }

        if (indices.IsComplete())
        {
            break;
        }

        i++;
    }

    return indices;
}

uint16_t GetVulkanDeviceScore(const VkPhysicalDevice &_physicalDevice)
{
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(_physicalDevice, &deviceProperties);
    vkGetPhysicalDeviceFeatures(_physicalDevice, &deviceFeatures);

    uint16_t score = 0;

    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
        score += 1000;
    }

    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(_physicalDevice, &memoryProperties);

    uint32_t heapCount = memoryProperties.memoryHeapCount;
    uint64_t totalDeviceMemory = 0;

    for (uint32_t j = 0; j < heapCount; ++j)
    {
        if (memoryProperties.memoryHeaps[j].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
        {
            totalDeviceMemory += memoryProperties.memoryHeaps[j].size;
        }
    }

    totalDeviceMemory /= 1024 * 1024;

    return score;
}

void CheckVulkanDeviceExtensionsSupport(VulkanDevice &_device)
{
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    for (const auto &requiredExtension : _device.requiredExtensions)
    {
        bool found = std::find_if(extensions.begin(), extensions.end(), [&](const VkExtensionProperties &extension) {
                         return strncmp(requiredExtension, extension.extensionName, VK_MAX_EXTENSION_NAME_SIZE);
                     }) != extensions.end();

        if (found)
        {
            _device.availableExtensions.push_back(requiredExtension);
        }
        else
        {
            throw std::runtime_error("Required Vulkan device extension is not supported!");
        }
    }

    for (const auto &optionalExtension : _device.optionalExtensions)
    {
        bool found = std::find_if(extensions.begin(), extensions.end(), [&](const VkExtensionProperties &extension) {
                         return strncmp(optionalExtension, extension.extensionName, VK_MAX_EXTENSION_NAME_SIZE);
                     }) != extensions.end();

        if (found)
        {
            _device.availableExtensions.push_back(optionalExtension);
        }
        else
        {
            std::cout << "WARNING: Optional Vulkan device extension is not supported: " << optionalExtension << '\n';
        }
    }
}

void CheckVulkanDeviceLayersSupport(VulkanDevice &_device)
{
    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> layers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

    for (const auto &requiredLayer : _device.requiredLayers)
    {
        bool found = std::find_if(layers.begin(), layers.end(), [&](const VkLayerProperties &layer) {
                         return strncmp(requiredLayer, layer.layerName, VK_MAX_EXTENSION_NAME_SIZE) == 0;
                     }) != layers.end();

        if (found)
        {
            _device.availableLayers.push_back(requiredLayer);
        }
        else
        {
            throw std::runtime_error("Required Vulkan device layer is not supported!");
        }
    }
}

std::vector<VkPhysicalDevice> EnumerateVulkanPhysicalDevices(const VulkanInstance &_instance)
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(_instance.instance, &deviceCount, nullptr);

    if (deviceCount == 0)
    {
        throw std::runtime_error("Failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(_instance.instance, &deviceCount, devices.data());

    return devices;
}

void PopulateVulkanDeviceProperties(const VkPhysicalDevice &physicalDevice, VulkanDeviceProperties &deviceProperties)
{
    VkPhysicalDeviceProperties vkDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &vkDeviceProperties);

    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

    std::ostringstream driverVersionStream;
    driverVersionStream << VK_VERSION_MAJOR(vkDeviceProperties.driverVersion) << "."
                        << VK_VERSION_MINOR(vkDeviceProperties.driverVersion) << "."
                        << VK_VERSION_PATCH(vkDeviceProperties.driverVersion);

    deviceProperties.vendorName = GetVulkanDeviceVendorNameString(vkDeviceProperties.vendorID);
    deviceProperties.deviceName = vkDeviceProperties.deviceName;
    deviceProperties.driverVersion = driverVersionStream.str();
    deviceProperties.memoryAmount = 0;

    for (uint32_t i = 0; i < memoryProperties.memoryHeapCount; ++i)
    {
        if (memoryProperties.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
        {
            deviceProperties.memoryAmount +=
                (int)(memoryProperties.memoryHeaps[i].size / (1024 * 1024)); // Convert to MB
        }
    }
}

std::vector<GpuInfoStruct> ListAllVulkanDevices(const VulkanInstance &_instance)
{
    std::vector<GpuInfoStruct> devicesInfo;
    std::vector<VkPhysicalDevice> devices = EnumerateVulkanPhysicalDevices(_instance);

    for (const auto &device : devices)
    {
        VulkanDeviceProperties properties;
        PopulateVulkanDeviceProperties(device, properties);

        devicesInfo.push_back(
            {properties.vendorName, properties.deviceName, properties.driverVersion, properties.memoryAmount});
    }

    return devicesInfo;
}

} // namespace gpu_info
