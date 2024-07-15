#include <iostream>

#include "vulkan_instance.hpp"

namespace gpu_info
{

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                    VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                    void *pUserData)
{
    switch (messageSeverity)
    {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        std::cout << "VERBOSE " << pCallbackData->pMessage << '\n';
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        std::cout << "INFO " << pCallbackData->pMessage << '\n';
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        std::cout << "WARNING " << pCallbackData->pMessage << '\n';
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        std::cout << "ERROR " << pCallbackData->pMessage << '\n';
        break;
    default:
        break;
    }
    return VK_FALSE;
}

static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                             const VkAllocationCallbacks *pAllocator,
                                             VkDebugUtilsMessengerEXT *pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                          const VkAllocationCallbacks *pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        func(instance, debugMessenger, pAllocator);
    }
}

static constexpr VkDebugUtilsMessengerCreateInfoEXT PopulateDebugMessengerCreateInfoEXT()
{
    return {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = debugCallback,
        .pUserData = nullptr,
    };
}

void CreateVulkanInstance(VulkanInstance &_instance)
{
    constexpr VkApplicationInfo appInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "Rake",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "Rake",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_3,
    };

    CheckVulkanInstanceExtensionsSupport(_instance);

    for (const auto &extension : _instance.availableExtensions)
    {
        std::cout << "INFO: Enabled Vulkan instance extension: " << extension << '\n';
    }

    CheckVulkanInstanceLayersSupport(_instance);

    for (const auto &layer : _instance.availableLayers)
    {
        std::cout << "INFO: Enabled Vulkan instance layer: " << layer << '\n';
    }

    void *debugCreateInfoPtr = nullptr;

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = PopulateDebugMessengerCreateInfoEXT();

    debugCreateInfoPtr = &debugCreateInfo;

    const VkInstanceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = debugCreateInfoPtr,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = static_cast<uint32_t>(_instance.availableLayers.size()),
        .ppEnabledLayerNames = _instance.availableLayers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(_instance.availableExtensions.size()),
        .ppEnabledExtensionNames = _instance.availableExtensions.data(),
    };

    if (vkCreateInstance(&createInfo, nullptr, &_instance.instance) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Vulkan instance!");
    }

    CreateVulkanDebugMessenger(_instance);
}

void DestroyVulkanInstance(VulkanInstance &_instance)
{
    if (_instance.debugMessenger != VK_NULL_HANDLE)
        DestroyVulkanDebugMessenger(_instance);

    if (_instance.instance != VK_NULL_HANDLE)
        vkDestroyInstance(_instance.instance, nullptr);
}

void CreateVulkanDebugMessenger(VulkanInstance &_instance)
{
    auto createInfo = PopulateDebugMessengerCreateInfoEXT();

    if (CreateDebugUtilsMessengerEXT(_instance.instance, &createInfo, nullptr, &_instance.debugMessenger) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Vulkan debug messenger!");
    }
}

void DestroyVulkanDebugMessenger(VulkanInstance &_instance)
{
    DestroyDebugUtilsMessengerEXT(_instance.instance, _instance.debugMessenger, nullptr);
}

void CheckVulkanInstanceExtensionsSupport(VulkanInstance &_instance)
{
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    for (const auto &requiredExtension : _instance.requiredExtensions)
    {
        bool found = std::find_if(extensions.begin(), extensions.end(), [&](const VkExtensionProperties &extension) {
                         return strncmp(requiredExtension, extension.extensionName, VK_MAX_EXTENSION_NAME_SIZE);
                     }) != extensions.end();

        if (found)
        {
            _instance.availableExtensions.push_back(requiredExtension);
        }
        else
        {
            throw std::exception("Required Vulkan instance extension is not supported!");
        }
    }

    for (const auto &optionalExtension : _instance.optionalExtensions)
    {
        bool found = std::find_if(extensions.begin(), extensions.end(), [&](const VkExtensionProperties &extension) {
                         return strncmp(optionalExtension, extension.extensionName, VK_MAX_EXTENSION_NAME_SIZE);
                     }) != extensions.end();

        if (found)
        {
            _instance.availableExtensions.push_back(optionalExtension);
        }
        else
        {
            std::cout << "WARNING: Optional Vulkan instance extension is not supported: " << optionalExtension << '\n';
        }
    }
}

void CheckVulkanInstanceLayersSupport(VulkanInstance &_instance)
{
    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> layers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

    for (const auto &requiredLayer : _instance.requiredLayers)
    {
        bool found = std::find_if(layers.begin(), layers.end(), [&](const VkLayerProperties &layer) {
                         return strncmp(requiredLayer, layer.layerName, VK_MAX_EXTENSION_NAME_SIZE) == 0;
                     }) != layers.end();

        if (found)
        {
            _instance.availableLayers.push_back(requiredLayer);
        }
        else
        {
            throw std::exception("Required Vulkan instance layer is not supported!");
        }
    }
}

} // namespace gpu_info
