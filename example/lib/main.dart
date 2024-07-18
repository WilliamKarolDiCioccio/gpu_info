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
  final List<GpuInfoStruct> _gpusInfo = [];

  @override
  void initState() {
    super.initState();
    initPlatformState();
  }

  Future<void> initPlatformState() async {
    List<GpuInfoStruct> gpusInfo;

    if (!mounted) return;

    gpusInfo = await _gpuInfoPlugin.getGpusInfo();

    setState(() {
      _gpusInfo.addAll(gpusInfo);
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
          child: Container(
            padding: const EdgeInsets.all(32),
            height: 300,
            child: ListView.builder(
              scrollDirection: Axis.horizontal,
              itemCount: _gpusInfo.length,
              itemBuilder: (context, index) {
                return Card(
                  child: Padding(
                    padding: const EdgeInsets.all(16.0),
                    child: Column(
                      children: [
                        Text(
                          'Your GPU: ${_gpusInfo[index].deviceName}',
                        ),
                        Text(
                          'Vendor: ${_gpusInfo[index].vendorName}',
                        ),
                        Text(
                          'Driver version: ${_gpusInfo[index].driverVersion}',
                        ),
                        Text(
                          'Memory: ${_gpusInfo[index].memoryAmount} MB',
                        ),
                      ],
                    ),
                  ),
                );
              },
            ),
          ),
        ),
      ),
    );
  }
}
