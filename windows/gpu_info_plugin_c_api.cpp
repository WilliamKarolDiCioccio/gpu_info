#include "include/gpu_info/gpu_info_plugin_c_api.h"

#include <flutter/plugin_registrar_windows.h>

#include "gpu_info_plugin.h"

void GpuInfoPluginCApiRegisterWithRegistrar(FlutterDesktopPluginRegistrarRef registrar)
{
    gpu_info::GpuInfoPlugin::RegisterWithRegistrar(
        flutter::PluginRegistrarManager::GetInstance()->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
