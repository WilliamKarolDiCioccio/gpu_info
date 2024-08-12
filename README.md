# 🚀 gpu_info

![Screenshot](https://github.com/WilliamKarolDiCioccio/gpu_info/blob/main/.github/images/screenshot.png)

The `gpu_info` Dart package allows you to query detailed GPU-related information using the Vulkan API. This package will soon support all platforms! 🌍

### 📢 Note:
This package is being developed as part of the larger [OpenLocalUI](https://github.com/WilliamKarolDiCioccio/open_local_ui) project. Check it out for more exciting features and updates! 🌟

## 📦 Installation

Add `gpu_info` as a dependency in your `pubspec.yaml` file:

```yaml
dependencies:
  gpu_info: ^0.1.0
```

### Vulkan SDK Installation

To build the `gpu_info` package, you'll need the Vulkan SDK installed on your system. Below are the commands to install the Vulkan SDK on different platforms:

#### **Windows**

Install the Vulkan SDK using `winget`:

```sh
winget install LunarG.VulkanSDK
```

Alternatively, you can download and install it directly from the [LunarG Vulkan SDK website](https://vulkan.lunarg.com/sdk/home).

#### **Linux**

For Ubuntu or Debian-based distributions, use `apt`:

```sh
sudo apt update
sudo apt install vulkan-sdk
```

For Fedora:

```sh
sudo dnf install vulkan-tools vulkan-loader-devel
```

For Arch Linux:

```sh
sudo pacman -S vulkan-tools vulkan-headers vulkan-validation-layers
```

#### **macOS**

Install the Vulkan SDK using `brew`:

```sh
brew install --cask vulkan-sdk
```

### Verifying the Installation

After installing the Vulkan SDK, you can verify the installation by running:

```sh
vulkaninfo
```

This command should output detailed information about your Vulkan setup, indicating that the SDK has been correctly installed.

## 🚀 Example

Here is a simple example of how to use the `gpu_info` package to retrieve and display GPU information in a Flutter app.

```dart
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
          title: const Text('GPU Info Example App'),
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
                      crossAxisAlignment: CrossAxisAlignment.start,
                      children: [
                        Text(
                          'GPU: ${_gpusInfo[index].deviceName}',
                          style: TextStyle(fontWeight: FontWeight.bold),
                        ),
                        SizedBox(height: 8),
                        Text('Vendor: ${_gpusInfo[index].vendorName}'),
                        Text('Driver version: ${_gpusInfo[index].driverVersion}'),
                        Text('Memory: ${_gpusInfo[index].memoryAmount} MB'),
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
```

### Explanation

This example demonstrates how to use the `gpu_info` package to retrieve and display information about the GPUs available on the system. The `GpuInfo` class is used to query the GPU information, and the results are displayed in a horizontal list view.

- **Fetching Data:** The `initPlatformState` method retrieves the GPU information asynchronously and updates the state.
- **Displaying Data:** The GPU information is displayed in a list view within a `Card` widget.

This example showcases a simple way to integrate the `gpu_info` package into your Flutter app and display GPU details.

For more detailed information, refer to the package documentation and explore the [OpenLocalUI](https://github.com/WilliamKarolDiCioccio/open_local_ui) project for more features and updates.
