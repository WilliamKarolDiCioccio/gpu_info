import 'package:flutter/services.dart';

import 'package:flutter_test/flutter_test.dart';

import 'package:gpu_info/gpu_info.dart';
import 'package:gpu_info/src/gpu_info_method_channel.dart';

void main() {
  TestWidgetsFlutterBinding.ensureInitialized();

  MethodChannelGpuInfo platform = MethodChannelGpuInfo();
  const MethodChannel channel = MethodChannel('gpu_info');

  setUp(() {
    TestDefaultBinaryMessengerBinding.instance.defaultBinaryMessenger.setMockMethodCallHandler(
      channel,
      (MethodCall methodCall) async {
        return '42';
      },
    );
  });

  tearDown(() {
    TestDefaultBinaryMessengerBinding.instance.defaultBinaryMessenger.setMockMethodCallHandler(channel, null);
  });

  test('getGpuInfo', () async {
    expect(await platform.getGpusInfo(), GpuInfoStruct('', '', '', 0));
  });
}
