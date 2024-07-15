#include <iostream>
#include <set>
#include <sstream>

#include "vulkan_device.hpp"

namespace gpu_info
{

void CreateVulkanDevice(VulkanDevice &_device, const VulkanInstance &_instance)
{
    VkPhysicalDevice physicalDevice = PickVulkanPhysicalDevice(_instance);

    CheckVulkanDeviceExtensionsSupport(_device);

    for (const auto &extension : _device.availableExtensions)
    {
        std::cout << "INFO: Enabled Vulkan device extension: " << extension << std::endl;
    }

    CheckVulkanDeviceLayersSupport(_device);

    for (const auto &layer : _device.availableLayers)
    {
        std::cout << "INFO: Enabled Vulkan device layer: " << layer << std::endl;
    }

    _device.physicalDevice = physicalDevice;

    auto indices = FindVulkanDeviceQueueFamiliesSupport(physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value()};

    float queuePriority = 1.0f;

    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    constexpr VkPhysicalDeviceFeatures deviceFeatures{
        .geometryShader = true,
        .sampleRateShading = true,
        .multiDrawIndirect = true,
        .fillModeNonSolid = true,
        .multiViewport = true,
        .samplerAnisotropy = true,
    };

    const VkDeviceCreateInfo deviceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
        .pQueueCreateInfos = queueCreateInfos.data(),
        .enabledLayerCount = static_cast<uint32_t>(_device.availableLayers.size()),
        .ppEnabledLayerNames = _device.availableLayers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(_device.availableExtensions.size()),
        .ppEnabledExtensionNames = _device.availableExtensions.data(),
        .pEnabledFeatures = &deviceFeatures,
    };

    if (vkCreateDevice(_device.physicalDevice, &deviceCreateInfo, nullptr, &_device.device) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Vulkan device!");
    }

    vkGetDeviceQueue(_device.device, indices.graphicsFamily.value(), 0, &_device.graphicsQueue);

    // Populate new fields with device information
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

    std::ostringstream driverVersionStream;
    driverVersionStream << VK_VERSION_MAJOR(deviceProperties.driverVersion) << "."
                        << VK_VERSION_MINOR(deviceProperties.driverVersion) << "."
                        << VK_VERSION_PATCH(deviceProperties.driverVersion);

    _device.vendorName = deviceProperties.deviceName;
    _device.deviceName = GetVulkanDeviceVendorNameString(deviceProperties.vendorID);
    _device.driverVersion = driverVersionStream.str();
    _device.memoryAmount = 0;

    for (uint32_t i = 0; i < memoryProperties.memoryHeapCount; ++i)
    {
        if (memoryProperties.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
        {
            _device.memoryAmount += (int)(memoryProperties.memoryHeaps[i].size / (1024 * 1024)); // Convert to MB
        }
    }
}

void DestroyVulkanDevice(VulkanDevice &_device)
{
    if (_device.device != VK_NULL_HANDLE)
        vkDestroyDevice(_device.device, nullptr);
}

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

VkPhysicalDevice PickVulkanPhysicalDevice(const VulkanInstance &_instance)
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(_instance.instance, &deviceCount, nullptr);

    if (deviceCount == 0)
    {
        throw std::runtime_error("Failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(_instance.instance, &deviceCount, devices.data());

    uint32_t highestScore = 0;
    VkPhysicalDevice physicalDevice = nullptr;

    for (const auto &device : devices)
    {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        if (GetVulkanDeviceScore(device) >= highestScore)
        {
            physicalDevice = device;
        }
    }

    if (physicalDevice == nullptr)
    {
        throw std::runtime_error("Failed to find a suitable GPU!");
    }

    return physicalDevice;
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

} // namespace gpu_info
