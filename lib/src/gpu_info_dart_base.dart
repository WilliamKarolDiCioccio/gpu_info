import 'package:gpu_info/src/gpu_info.dart';

import 'gpu_info_platform_interface.dart';

class GpuInfo {
  Future<List<GpuInfoStruct>> getGpusInfo() {
    return GpuInfoPlatform.instance.getGpusInfo();
  }
}
