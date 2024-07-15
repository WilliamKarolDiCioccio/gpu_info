import 'package:flutter_test/flutter_test.dart';
import 'package:gpu_info/src/gpu_info.dart';
import 'package:gpu_info/src/gpu_info_dart_base.dart';
import 'package:gpu_info/src/gpu_info_platform_interface.dart';
import 'package:gpu_info/src/gpu_info_method_channel.dart';
import 'package:plugin_platform_interface/plugin_platform_interface.dart';

class MockGpuInfoPlatform
    with MockPlatformInterfaceMixin
    implements GpuInfoPlatform {
  @override
  Future<GpuInfoStruct> getGpuInfo() {
    return Future.value(GpuInfoStruct('', '', '', 0));
  }
}

void main() {
  final GpuInfoPlatform initialPlatform = GpuInfoPlatform.instance;

  test('$MethodChannelGpuInfo is the default instance', () {
    expect(initialPlatform, isInstanceOf<MethodChannelGpuInfo>());
  });

  test('getPlatformVersion', () async {
    GpuInfo gpuInfoPlugin = GpuInfo();
    MockGpuInfoPlatform fakePlatform = MockGpuInfoPlatform();
    GpuInfoPlatform.instance = fakePlatform;

    expect(await gpuInfoPlugin.getGpuInfo(), GpuInfoStruct('', '', '', 0));
  });
}
