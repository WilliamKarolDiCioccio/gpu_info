import 'dart:async';

import 'package:flutter/material.dart';

import 'package:gpu_info/gpu_info.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatefulWidget {
  const MyApp({super.key});

  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  final _gpuInfoPlugin = GpuInfo();
  GpuInfoStruct _gpuInfo = GpuInfoStruct('', '', '', 0);

  @override
  void initState() {
    super.initState();
    initPlatformState();
  }

  Future<void> initPlatformState() async {
    GpuInfoStruct gpuInfo;

    if (!mounted) return;

    gpuInfo = await _gpuInfoPlugin.getGpuInfo();

    setState(() {
      _gpuInfo = gpuInfo;
    });
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('Plugin example app'),
        ),
        body: Center(
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              Text('Your GPU: ${_gpuInfo.deviceName}'),
              Text('Vendor: ${_gpuInfo.vendorName}'),
              Text('Driver version: ${_gpuInfo.driverVersion}'),
              Text('Memory: ${_gpuInfo.memoryAmount} MB'),
            ],
          ),
        ),
      ),
    );
  }
}
