#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan.h>

#include "gpu_info.hpp"
#include "vulkan_instance.hpp"

namespace gpu_info
{

struct VulkanQueueFamilySupportDetails
{
    std::optional<uint32_t> graphicsFamily;

    bool IsComplete()
    {
        return graphicsFamily.has_value();
    }
};

struct VulkanDevice
{
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    std::vector<const char *> requiredExtensions;
    std::vector<const char *> optionalExtensions;
    std::vector<const char *> availableExtensions;
    std::vector<const char *> requiredLayers;
    std::vector<const char *> availableLayers;

    std::vector<GpuInfoStruct> devicesProperties;

    VulkanDevice()
        : physicalDevice(VK_NULL_HANDLE), device(VK_NULL_HANDLE), graphicsQueue(VK_NULL_HANDLE),
          presentQueue(VK_NULL_HANDLE)
    {
        requiredExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_KHR_MULTIVIEW_EXTENSION_NAME,
        };

        requiredLayers = {
            "VK_LAYER_KHRONOS_validation",
        };
    }
};

VulkanQueueFamilySupportDetails FindVulkanDeviceQueueFamiliesSupport(const VkPhysicalDevice &_device);

uint16_t GetVulkanDeviceScore(const VkPhysicalDevice &_device);

void CheckVulkanDeviceExtensionsSupport(VulkanDevice &_device);

void CheckVulkanDeviceLayersSupport(VulkanDevice &_device);

std::vector<VkPhysicalDevice> EnumerateVulkanPhysicalDevices(const VulkanInstance &_instance);

GpuInfoStruct PopulateDeviceInfos(const VkPhysicalDevice &physicalDevice);

std::vector<GpuInfoStruct> GetAllVulkanDevices(const VulkanInstance &_instance);

inline std::string GetVulkanDeviceTypeString(VkPhysicalDeviceType _type)
{
    switch (_type)
    {
    case VK_PHYSICAL_DEVICE_TYPE_OTHER:
        return "Other";
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
        return "Integrated GPU";
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
        return "Discrete GPU";
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
        return "Virtual GPU";
    case VK_PHYSICAL_DEVICE_TYPE_CPU:
        return "CPU";
    default:
        return "Unknown";
    }
}

inline std::string GetVulkanDeviceVendorNameString(uint32_t vendorID)
{
    static const std::unordered_map<uint32_t, std::string> vendorMap = {
        {0x1002, "AMD"}, {0x1010, "ImgTec"},   {0x10DE, "NVIDIA"},
        {0x13B5, "ARM"}, {0x5143, "Qualcomm"}, {0x8086, "Intel"},
    };

    auto it = vendorMap.find(vendorID);
    if (it != vendorMap.end())
    {
        return it->second;
    }
    else
    {
        return "Unknown Vendor";
    }
}

} // namespace gpu_info
