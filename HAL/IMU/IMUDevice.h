#pragma once

#include <HAL/IMU/IMUDriverInterface.h>
#include <HAL/Devices/DriverFactory.h>
#include <HAL/Utils/Uri.h>

namespace hal {

  // Generic IMU device
  class IMU : public IMUDriverInterface {
    public:
      IMU() {}

      IMU(const std::string& uri) : m_URI(uri) {
        m_IMU = DeviceDriverRegistry<IMUDriverInterface>::Instance().Create(m_URI);
      }

      ~IMU() {
        Clear();
      }

      inline void Reset() {
        Clear();
        m_IMU = DeviceDriverRegistry<IMUDriverInterface>::Instance().Create(m_URI);
        RegisterIMUDataCallback(m_callback);
      }

      void Clear() {
        m_IMU = nullptr;
      }

      void RegisterIMUDataCallback(IMUDriverDataCallback callback) {
        m_callback = callback;
        if( m_IMU ){
          m_IMU->RegisterIMUDataCallback( callback );
        }else{
          std::cerr << "ERROR: no driver initialized!\n";
        }
        return;
      }

      bool IsRunning() const override {
        return m_IMU->IsRunning();
      }

    protected:
      hal::Uri                                m_URI;
      std::shared_ptr<IMUDriverInterface>     m_IMU;
      IMUDriverDataCallback m_callback;
  };
} /* namespace hal */
