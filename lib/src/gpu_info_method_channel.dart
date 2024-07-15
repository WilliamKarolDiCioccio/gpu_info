import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';

import 'package:gpu_info/src/gpu_info.dart';

import 'gpu_info_platform_interface.dart';

class MethodChannelGpuInfo extends GpuInfoPlatform {
  @visibleForTesting
  final methodChannel = const MethodChannel('gpu_info');

  @override
  Future<GpuInfoStruct> getGpuInfo() async {
    final result = await methodChannel.invokeMethod<Map<dynamic, dynamic>?>('getGpuInfo');
    
    if (result == null) {
      throw PlatformException(
          code: 'NULL_RESULT',
          message: 'Received null result from method channel.',
      );
    }

    final Map<String, dynamic> castedResult = result.map(
      (key, value) => MapEntry(key as String, value),
    );

    return GpuInfoStruct.fromMap(castedResult);
  }
}
