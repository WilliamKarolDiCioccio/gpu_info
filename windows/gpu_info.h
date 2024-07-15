#include <map>
#include <any>
#include <string>

namespace gpu_info
{

    struct GPUInfo
    {
        std::string vendorName;
        std::string deviceName;
        std::string driverVersion;
        int memoryAmount;

        std::map<std::string, std::any> toMap() const
        {
            return {
                {"deviceName", deviceName},
                {"vendorName", vendorName},
                {"driverVersion", driverVersion},
                {"memoryAmount", memoryAmount}};
        }
    };

}
