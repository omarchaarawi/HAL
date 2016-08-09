#pragma once
#include <functional>
#include <HAL/Devices/DriverInterface.h>
#include <HAL/Car.pb.h>

namespace hal {
typedef std::function<void (hal::CarStateMsg&)> CarStateDriverDataCallback;

///////////////////////////////////////////////////////////////////////////////
// Generic Car Driver Interface
///////////////////////////////////////////////////////////////////////////////
class CarDriverInterface : public DriverInterface
{
public:
  virtual ~CarDriverInterface() {}

  virtual bool ApplyCommand( double flMotorCurrent, double flSteering) = 0;

  virtual void RegisterCarStateDataCallback(CarStateDriverDataCallback callback) = 0;
};

}
