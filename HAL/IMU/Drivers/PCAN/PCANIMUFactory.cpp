#include <HAL/Devices/DriverFactory.h>

#include "PCANIMUDriver.h"

namespace hal
{

class PCANIMUFactory : public DriverFactory<IMUDriverInterface>
{
public:
    PCANIMUFactory(const std::string& name)
        : DriverFactory<IMUDriverInterface>(name)
    {
        Params() = {
        {"portname", "/dev/pcan32", "Port name such as /dev/pcan32"},
        {"baudrate", "500000", "CAN Bus baudrate as 125000 or 500000 or1000000"}
        };
    }

    std::shared_ptr<IMUDriverInterface> CreateDriver(const Uri& uri)
    {
        std::string portname  = uri.properties.Get("portname", std::string("/dev/pcan32"));
        int baudrate = uri.properties.Get("baudrate", 500000);
        PCANIMUDriver* pDriver = new PCANIMUDriver( baudrate, portname );
        return std::shared_ptr<IMUDriverInterface>( pDriver );
    }
};

// Register this factory by creating static instance of factory
static PCANIMUFactory g_PCANIMUFactory("pcan");

}
