import 'package:plugin_platform_interface/plugin_platform_interface.dart';

import 'package:gpu_info/src/gpu_info.dart';

import 'gpu_info_method_channel.dart';

abstract class GpuInfoPlatform extends PlatformInterface {
  GpuInfoPlatform() : super(token: _token);

  static final Object _token = Object();

  static GpuInfoPlatform _instance = MethodChannelGpuInfo();

  static GpuInfoPlatform get instance => _instance;

  static set instance(GpuInfoPlatform instance) {
    PlatformInterface.verifyToken(instance, _token);
    _instance = instance;
  }

  Future<GpuInfoStruct> getGpuInfo() {
    throw UnimplementedError('getGpuInfo() has not been implemented.');
  }
}
