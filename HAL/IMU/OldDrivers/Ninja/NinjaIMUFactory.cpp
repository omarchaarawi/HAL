#include <HAL/Devices/DriverFactory.h>

#include "NinjaIMUDriver.h"

namespace hal
{

class NinjaIMUFactory : public DriverFactory<IMUDriverInterface>
{
public:
    NinjaIMUFactory(const std::string& name)
        : DriverFactory<IMUDriverInterface>(name)
    {
        Params() = {
        };
    }

    std::shared_ptr<IMUDriverInterface> CreateDriver(const Uri& uri)
    {
        NinjaIMUDriver* pDriver = new NinjaIMUDriver( uri.url );
        return std::shared_ptr<IMUDriverInterface>( pDriver );
    }
};

// Register this factory by creating static instance of factory
static NinjaIMUFactory g_NinjaIMUFactory("ninja");

}