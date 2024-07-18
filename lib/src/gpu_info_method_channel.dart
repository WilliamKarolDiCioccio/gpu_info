import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';

import 'package:gpu_info/src/gpu_info.dart';

import 'gpu_info_platform_interface.dart';

class MethodChannelGpuInfo extends GpuInfoPlatform {
  @visibleForTesting
  final methodChannel = const MethodChannel('gpu_info');

  @override
  Future<List<GpuInfoStruct>> getGpusInfo() async {
    final results = await methodChannel.invokeMethod<dynamic>('getGpusInfo');

    if (results == null) {
      throw PlatformException(
        code: 'NULL_RESULT',
        message: 'Received null result from method channel.',
      );
    }

    final List<GpuInfoStruct> castedResult = [];

    for (final element in results) {
      if (element is Map) {
        castedResult.add(
          GpuInfoStruct.fromMap(Map<String, dynamic>.from(element)),
        );
      } else {
        throw PlatformException(
          code: 'INVALID_RESULT',
          message: 'Received invalid result from method channel.',
        );
      }
    }

    return castedResult;
  }
}
