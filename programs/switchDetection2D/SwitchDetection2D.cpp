// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "SwitchDetection2D.hpp"

#include <cstdio>
#include <string>
#include <iostream>

#include <yarp/os/Property.h>
#include <yarp/os/Time.h>

#include <ColorDebug.h>

using namespace roboticslab;

/************************************************************************/

bool SwitchDetection2D::configure(yarp::os::ResourceFinder &rf)
{
    cropSelector = DEFAULT_CROP_SELECTOR;
    std::string strCameraDevice = DEFAULT_CAMERA_DEVICE;
    std::string strCameraLocal = DEFAULT_CAMERA_LOCAL;
    std::string strCameraRemote = DEFAULT_CAMERA_REMOTE;
    std::string strSwitchMode = DEFAULT_SWITCH_MODE;
    watchdog = DEFAULT_WATCHDOG;  // double


    if (rf.check("help"))
    {
        std::printf("SwitchDetection2D options:\n");
        std::printf("\t--help (this help)\t--from [file.ini]\t--context [path]\n");
        std::printf("\t--cropSelector (default: \"%d\")\n", cropSelector);
        std::printf("\t--cameraDevice (device we create, default: \"%s\")\n", strCameraDevice.c_str());
        std::printf("\t--cameraLocal (if accesing remote, local port name, default: \"%s\")\n", strCameraLocal.c_str());
        std::printf("\t--cameraRemote (if accesing remote, remote port name, default: \"%s\")\n", strCameraRemote.c_str());
        std::printf("\t--switchMode (default: \"%s\")\n", strSwitchMode.c_str());
        std::printf("\t--watchdog ([s] default: \"%f\")\n", watchdog);
        // Do not exit: let last layer exit so we get help from the complete chain.
    }

    if (rf.check("cropSelector"))
    {
        cropSelector = rf.find("cropSelector").asInt32();
    }

    CD_INFO("Using cropSelector: %d.\n", cropSelector);

    if (rf.check("cameraDevice"))
    {
        strCameraDevice = rf.find("cameraDevice").asString();
    }

    if (rf.check("cameraLocal"))
    {
        strCameraLocal = rf.find("cameraLocal").asString();
    }

    if (rf.check("cameraRemote"))
    {
        strCameraRemote = rf.find("cameraRemote").asString();
    }

    if (rf.check("watchdog"))
    {
        watchdog = rf.find("watchdog").asFloat64();
    }

    if (rf.check("switchMode"))
    {
        strSwitchMode = rf.find("switchMode").asString();

        if(strSwitchMode!="haarDetection"||strSwitchMode!="tensorflowDetection"||strSwitchMode!="colorRegionDetection"){
          std::cout<<strSwitchMode<<" mode not allowed"<<std::endl;
          std::exit(1);
        }

        strCameraLocal ="/"+strSwitchMode+"2D";
        rf.setDefaultContext(strSwitchMode);
    }

    CD_INFO("Using cameraDevice: %s, cameraLocal: %s, cameraRemote: %s.\n",
        strCameraDevice.c_str(), strCameraLocal.c_str(), strCameraRemote.c_str());
    CD_INFO("Using switchMode: %s.\n", strSwitchMode.c_str());
    CD_INFO("Using watchdog: %f.\n", watchdog);


    if (!rf.check("help"))
    {
        yarp::os::Property options;
        options.fromString(rf.toString());
        options.put("device", strCameraDevice);
        options.put("local", strCameraLocal);
        options.put("remote", strCameraRemote);

        while (!dd.open(options))
        {
            CD_INFO("Waiting for camera device \"%s\"...\n", strCameraDevice.c_str());
            yarp::os::Time::delay(1);
        }

        CD_SUCCESS("Camera device available.\n");

        if (!dd.view(camera))
        {
            CD_WARNING("Camera device bad view.\n");
        }
        else
        {
            CD_SUCCESS("Camera device ok view.\n");
        }

        segmentorThread.setIFrameGrabberImageDriver(camera);
        segmentorThread.setOutImg(&outImg);
        segmentorThread.setOutPort(&outPort);
        segmentorThread.setCropSelector(cropSelector);

        if (cropSelector != 0)
        {
            segmentorThread.setOutCropSelectorImg(&outCropSelectorImg);
            segmentorThread.setInCropSelectorPort(&inCropSelectorPort);
        }
    }

    segmentorThread.init(rf);

    //-----------------OPEN LOCAL PORTS------------//

    outImg.open(strCameraLocal + "/img:o");
    outPort.open(strCameraLocal + "/state:o");

    if (cropSelector != 0)
    {
        outCropSelectorImg.open(strCameraLocal + "/cropSelector/img:o");
        inCropSelectorPort.open(strCameraLocal + "/cropSelector/state:i");
    }

    return true;
}

/*****************************************************************/

double SwitchDetection2D::getPeriod()
{
    return watchdog;  // [s]
}

/************************************************************************/

bool SwitchDetection2D::updateModule()
{
    CD_INFO("Alive...\n");
    return true;
}

/************************************************************************/

bool SwitchDetection2D::interruptModule()
{
    outImg.interrupt();
    outPort.interrupt();

    if (cropSelector != 0)
    {
        outCropSelectorImg.interrupt();
        inCropSelectorPort.interrupt();
    }

    return true;
}

/************************************************************************/

bool SwitchDetection2D::close()
{
    CD_INFO("Closing...\n");

    segmentorThread.stop();

    dd.close();
    outImg.close();
    outPort.close();

    if (cropSelector != 0)
    {
        outCropSelectorImg.close();
        inCropSelectorPort.close();
    }

    return true;
}

/************************************************************************/
