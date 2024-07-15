#include <flutter/method_call.h>
#include <flutter/method_result_functions.h>
#include <flutter/standard_method_codec.h>
#include <gtest/gtest.h>
#include <windows.h>

#include <memory>
#include <string>
#include <variant>

#include "gpu_info_plugin.h"

namespace gpu_info {
namespace test {

namespace {

using flutter::EncodableMap;
using flutter::EncodableValue;
using flutter::MethodCall;
using flutter::MethodResultFunctions;

}  // namespace

TEST(GpuInfoPlugin, GetPlatformVersion) {
  GpuInfoPlugin plugin;
  // Save the reply value from the success callback.
  std::string result_string;
  plugin.HandleMethodCall(
      MethodCall("getGpuInfo", std::make_unique<EncodableValue>()),
      std::make_unique<MethodResultFunctions<>>(
          [&result_string](const EncodableValue* result) {
            result_string = std::get<std::string>(*result);
          },
          nullptr, nullptr));
}

}  // namespace test
}  // namespace go