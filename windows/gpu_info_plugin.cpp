#include "gpu_info_plugin.h"

#include <windows.h>

#include <VersionHelpers.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <memory>
#include <sstream>

#include "gpu_info.h"
#include "vulkan_device.hpp"
#include "vulkan_instance.hpp"

namespace gpu_info
{

void GpuInfoPlugin::RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar)
{
    auto channel = std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
        registrar->messenger(), "gpu_info", &flutter::StandardMethodCodec::GetInstance());

    auto plugin = std::make_unique<GpuInfoPlugin>();

    channel->SetMethodCallHandler([plugin_pointer = plugin.get()](const auto &call, auto result) {
        plugin_pointer->HandleMethodCall(call, std::move(result));
    });

    registrar->AddPlugin(std::move(plugin));
}

GpuInfoPlugin::GpuInfoPlugin()
{
}

GpuInfoPlugin::~GpuInfoPlugin()
{
}

void GpuInfoPlugin::HandleMethodCall(const flutter::MethodCall<flutter::EncodableValue> &method_call,
                                     std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result)
{
    if (method_call.method_name().compare("getGpuInfo") == 0)
    {
        GPUInfo gpuInfo;

        VulkanInstance vulkanInstance;
        CreateVulkanInstance(vulkanInstance);

        VulkanDevice vulkanDevice;
        CreateVulkanDevice(vulkanDevice, vulkanInstance);

        gpuInfo.deviceName = vulkanDevice.deviceName;
        gpuInfo.vendorName = vulkanDevice.vendorName;
        gpuInfo.driverVersion = vulkanDevice.driverVersion;
        gpuInfo.memoryAmount = vulkanDevice.memoryAmount;

        std::map<std::string, std::any> gpuInfoMap = gpuInfo.toMap();

        flutter::EncodableMap encodableMap;

        for (const auto &[key, value] : gpuInfoMap)
        {
            if (value.type() == typeid(std::string))
            {
                encodableMap[flutter::EncodableValue(key)] = flutter::EncodableValue(std::any_cast<std::string>(value));
            }
            else if (value.type() == typeid(int))
            {
                encodableMap[flutter::EncodableValue(key)] = flutter::EncodableValue(std::any_cast<int>(value));
            }
            else if (value.type() == typeid(float))
            {
                encodableMap[flutter::EncodableValue(key)] = flutter::EncodableValue(std::any_cast<float>(value));
            }
        }

        DestroyVulkanDevice(vulkanDevice);
        DestroyVulkanInstance(vulkanInstance);

        result->Success(encodableMap);
    }
    else
    {
        result->NotImplemented();
    }
}

} // namespace gpu_info
