#include "gpu_info_plugin.h"

#include <windows.h>

#include <VersionHelpers.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <memory>
#include <sstream>

#include "gpu_info.hpp"
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
    if (method_call.method_name().compare("getGpusInfo") == 0)
    {
        VulkanInstance vulkanInstance;
        CreateVulkanInstance(vulkanInstance);

        auto devicesInfo = ListAllVulkanDevices(vulkanInstance);

        flutter::EncodableList encodableList;

        for (const auto &element : devicesInfo)
        {
            flutter::EncodableMap encodableMap;

            auto deviceMap = element.toMap();
            for (const auto &[key, value] : deviceMap)
            {
                std::string keyStr = std::any_cast<std::string>(key);
                if (value.type() == typeid(std::string))
                {
                    encodableMap[flutter::EncodableValue(keyStr)] =
                        flutter::EncodableValue(std::any_cast<std::string>(value));
                }
                else if (value.type() == typeid(int))
                {
                    encodableMap[flutter::EncodableValue(keyStr)] = flutter::EncodableValue(std::any_cast<int>(value));
                }
                else if (value.type() == typeid(float))
                {
                    encodableMap[flutter::EncodableValue(keyStr)] =
                        flutter::EncodableValue(std::any_cast<float>(value));
                }
            }

            encodableList.push_back(encodableMap);
        }

        DestroyVulkanInstance(vulkanInstance);

        result->Success(flutter::EncodableValue(encodableList));
    }
    else
    {
        result->NotImplemented();
    }
}

} // namespace gpu_info
