#pragma once

#include <vector>

#include <vulkan/vulkan.h>

namespace gpu_info
{

struct VulkanInstance
{
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    std::vector<const char *> requiredExtensions;
    std::vector<const char *> optionalExtensions;
    std::vector<const char *> availableExtensions;
    std::vector<const char *> requiredLayers;
    std::vector<const char *> availableLayers;

    VulkanInstance() : instance(VK_NULL_HANDLE), debugMessenger(VK_NULL_HANDLE)
    {
        requiredExtensions = {};

        requiredExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        requiredExtensions.emplace_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

        requiredLayers = {
            "VK_LAYER_KHRONOS_validation",
        };
    }
};

void CreateVulkanInstance(VulkanInstance &_instance);

void DestroyVulkanInstance(VulkanInstance &_instance);

void CreateVulkanDebugMessenger(VulkanInstance &_instance);

void DestroyVulkanDebugMessenger(VulkanInstance &_instance);

void CheckVulkanInstanceExtensionsSupport(VulkanInstance &_instance);

void CheckVulkanInstanceLayersSupport(VulkanInstance &_instance);

} // namespace gpu_info
