#pragma once

#include <vrpn_Tracker.h>

#include <HAL/Posys/PosysDriverInterface.h>

namespace hal {


class ViconDriver : public PosysDriverInterface
{
public:
    ViconDriver(std::string sHost, std::string sTrackedObjs);
    ~ViconDriver();
    void RegisterPosysDataCallback(PosysDriverDataCallback callback);

private:
    static void VRPN_CALLBACK _ViconHandler(void* uData, const vrpn_TRACKERCB tData);

private:
    PosysDriverDataCallback                     m_Callback;

    struct TrackerObject
    {
        std::string                             m_sName;
        vrpn_Tracker_Remote*                    m_pTracker;
        ViconDriver*                            m_pViconDriver;
    };

    std::map< std::string, TrackerObject >      m_mObjects;
    vrpn_Connection*                            m_pViconConnection;

};

} /* namespace */