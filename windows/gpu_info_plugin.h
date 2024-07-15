#ifndef FLUTTER_PLUGIN_GPU_INFO_PLUGIN_H_
#define FLUTTER_PLUGIN_GPU_INFO_PLUGIN_H_

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>

#include <memory>

namespace gpu_info
{

class GpuInfoPlugin : public flutter::Plugin
{
  public:
    static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

    GpuInfoPlugin();

    virtual ~GpuInfoPlugin();

    GpuInfoPlugin(const GpuInfoPlugin &) = delete;
    GpuInfoPlugin &operator=(const GpuInfoPlugin &) = delete;

    void HandleMethodCall(const flutter::MethodCall<flutter::EncodableValue> &method_call,
                          std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
};

} // namespace gpu_info

#endif // FLUTTER_PLUGIN_GPU_INFO_PLUGIN_H_
