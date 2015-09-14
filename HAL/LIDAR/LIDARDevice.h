#pragma once

#include <HAL/LIDAR/LIDARDriverInterface.h>
#include <HAL/Devices/DriverFactory.h>
#include <HAL/Utils/Uri.h>

namespace hal {

///////////////////////////////////////////////////////////////////////////////
// Generic LIDAR device
class LIDAR : public LIDARDriverInterface
{
    public:
        ///////////////////////////////////////////////////////////////
        LIDAR()
        {
        }

        ///////////////////////////////////////////////////////////////
        LIDAR(const std::string& uri)
            : m_URI(uri)
        {
          m_LIDAR = DeviceRegistry<LIDARDriverInterface>::Instance().Create(m_URI);
        }

        ///////////////////////////////////////////////////////////////
        ~LIDAR()
        {
            Clear();
        }

        ///////////////////////////////////////////////////////////////
        void Clear()
        {
            m_LIDAR = nullptr;
        }

        ///////////////////////////////////////////////////////////////
        void RegisterLIDARDataCallback(LIDARDriverDataCallback callback)
        {
            if( m_LIDAR ){
                m_LIDAR->RegisterLIDARDataCallback( callback );
            }else{
                std::cerr << "ERROR: no driver initialized!\n";
            }
            return;
        }

        /*
        ///////////////////////////////////////////////////////////////
        std::string GetProperty(const std::string& sProperty)
        {
            return m_LIDAR->GetProperty(sProperty);
        }
        */


protected:
    hal::Uri                                m_URI;
    std::shared_ptr<LIDARDriverInterface>     m_LIDAR;

};

} /* namespace */
