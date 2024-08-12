class GpuInfoStruct {
  final String deviceName;
  final String vendorName;
  final String driverVersion;
  final int memoryAmount;
  final int deviceScore;

  GpuInfoStruct(
    this.deviceName,
    this.vendorName,
    this.driverVersion,
    this.memoryAmount,
    this.deviceScore,
  );

  factory GpuInfoStruct.fromMap(Map<String, dynamic> map) {
    return GpuInfoStruct(
      map['deviceName'] as String,
      map['vendorName'] as String,
      map['driverVersion'] as String,
      map['memoryAmount'] as int,
      map['deviceScore'] as int,
    );
  }
}
