#include <pangolin/pangolin.h>
#include <pangolin/timer.h>

#include <HAL/Camera/CameraDevice.h>
#include <HAL/IMU/IMUDevice.h>
#include <HAL/Posys/PosysDevice.h>

#include <HAL/Utils/GetPot>
#include <HAL/Utils/TicToc.h>

#include <PbMsgs/Logger.h>
#include <PbMsgs/Matrix.h>


bool        g_bLog      = false;
pb::Logger& g_Logger    = pb::Logger::GetInstance();


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void IMU_Handler(pb::ImuMsg& IMUdata)
{
    if( g_bLog ) {
        pb::Msg pbMsg;
        pbMsg.set_timestamp( hal::Tic() );
        pbMsg.mutable_imu()->Swap(&IMUdata);
        g_Logger.LogMessage(pbMsg);
    }
//    const pb::VectorMsg& pbVec = IMUdata.accel();
//    printf("X: %5f    Y: %5f     Z: %5f\r",pbVec.data(0),pbVec.data(1),pbVec.data(2));
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Posys_Handler(pb::PoseMsg& PoseData)
{
    std::cout << "Got at time: " << PoseData.device_time() << std::endl;

    pb::VectorMsg pbVec = PoseData.pose();

    for( int ii = 0; ii < pbVec.data_size(); ++ii ) {
        std::cout << "Data " << ii << ": " << pbVec.data(ii) << std::endl;
    }

    if( g_bLog ) {
        pb::Msg pbMsg;
        pbMsg.set_timestamp( hal::Tic() );
        pbMsg.mutable_pose()->Swap(&PoseData);
        g_Logger.LogMessage(pbMsg);
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main( int argc, char* argv[] )
{
    GetPot clArgs(argc,argv);

    ///-------------------- CAMERA INIT (Optional)

    std::string sCam = clArgs.follow("", "-cam" );
    hal::Camera theCam;
    
    // N cameras, each w*h in dimension, greyscale
    size_t nNumChannels = 0;
    size_t nImgWidth = 0;
    size_t nImgHeight = 0;

    pb::ImageArray vImgs;
    
    if(!sCam.empty()) {
        theCam = hal::Camera(sCam);
        nNumChannels = theCam.NumChannels();
        nImgWidth = theCam.Width();
        nImgHeight = theCam.Height();        

        std::cout << "- Opening camera with " << nNumChannels << " channel(s)." << std::endl;
        for(size_t ii=0; ii<nNumChannels; ++ii) {
            std::cout << "  " << theCam.Width(ii) << "x" << theCam.Height(ii) << std::endl;
        }
    }

    ///-------------------- IMU INIT (Optional)

    std::string sIMU = clArgs.follow("", "-imu" );
    const bool bHaveIMU = !sIMU.empty();

    hal::IMU theIMU;
    if( bHaveIMU ) {
        theIMU = hal::IMU(sIMU);
        theIMU.RegisterIMUDataCallback(IMU_Handler);
        std::cout << "- Registering IMU device." << std::endl;
    }


    ///-------------------- POSYS INIT (Optional)

    std::string sPosys = clArgs.follow("", "-posys" );
    const bool bHavePosys = !sPosys.empty();

    hal::Posys thePosys;
    if( bHavePosys ) {
        thePosys = hal::Posys(sPosys);
        thePosys.RegisterPosysDataCallback(Posys_Handler);
        std::cout << "- Registering Posys device." << std::endl;
    }


    ///-------------------- WINDOW INIT

    // Setup OpenGL Display (based on GLUT)
    pangolin::CreateWindowAndBind(__FILE__,nNumChannels*nImgWidth,nImgHeight);

    glPixelStorei(GL_PACK_ALIGNMENT,1);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    pangolin::GlTexture glTex;

    // Create Smart viewports for each camera image that preserve aspect
    pangolin::View& ContainerView = pangolin::CreateDisplay().SetLayout(pangolin::LayoutEqual);
    for(size_t ii=0; ii < nNumChannels; ++ii ) {
        ContainerView.AddDisplay(pangolin::CreateDisplay().SetAspect((double)nImgWidth/nImgHeight));
    }

    bool bRun = true;
    bool bStep = false;

    pangolin::RegisterKeyPressCallback(pangolin::PANGO_SPECIAL + pangolin::PANGO_KEY_RIGHT, [&bStep](){bStep=true;} );
    pangolin::RegisterKeyPressCallback(' ', [&](){bRun = !bRun;} );

    pangolin::RegisterKeyPressCallback('l', [&](){ g_bLog = !g_bLog; } );

    pangolin::Timer theTimer;

    for(unsigned long nFrame=0; !pangolin::ShouldQuit(); nFrame++)
    {
        const bool bGo = bRun || pangolin::Pushed(bStep);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glColor4f(1.0f,1.0f,1.0f,1.0f);

        if(bGo && nNumChannels) {
            if( !theCam.Capture(vImgs) ) {
                bRun = false;
            }

#ifdef HAVE_GLUT
            if(nFrame%30 == 0) {
                char buffer[1024];
                sprintf(buffer,"CameraViewer (FPS: %f)", 30.0 / theTimer.Elapsed_s() );
                glutSetWindowTitle(buffer);
                theTimer.Reset();
            }
#endif
        }

        if(!glTex.tid && nNumChannels) {
            // Only initialise now we know format.
            glTex.Reinitialise(
                nImgWidth, nImgHeight, vImgs[0].Format(), true, 0,
                vImgs[0].Format(),vImgs[0].Type(), 0
            );
        }

        for(size_t ii=0; ii<nNumChannels; ++ii ) {
            ContainerView[ii].Activate();
            glTex.Upload( vImgs[ii].data(), vImgs[ii].Format(), vImgs[ii].Type() );
            glTex.RenderToViewportFlipY();
        }

        if(g_bLog && bRun) {
            pb::Msg pbMsg;
            pbMsg.set_timestamp( hal::Tic() );
            pbMsg.mutable_camera()->Swap(&vImgs.Ref());
            g_Logger.LogMessage(pbMsg);

            // draw red circle on bottom left corner for visual cue
            ContainerView.ActivatePixelOrthographic();
            glPushAttrib(GL_ENABLE_BIT);
            glDisable(GL_LIGHTING);
            glDisable(GL_DEPTH_TEST);
            glColor3ub( 255, 0, 0 );
            pangolin::glDrawCircle(20,20,7);
            glPopAttrib();
        }

        pangolin::FinishFrame();
    }
}
