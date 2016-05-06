// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "ColorRegionDetection.hpp"

namespace teo
{

/************************************************************************/
bool ColorRegionDetection::configure(yarp::os::ResourceFinder &rf) {

    cropSelector = DEFAULT_CROP_SELECTOR;
    yarp::os::ConstString strKinectDevice = DEFAULT_KINECT_DEVICE;
    yarp::os::ConstString strKinectLocal = DEFAULT_KINECT_LOCAL;
    yarp::os::ConstString strKinectRemote = DEFAULT_KINECT_REMOTE;
    watchdog = DEFAULT_WATCHDOG;  // double

    fprintf(stdout,"--------------------------------------------------------------\n");
    if (rf.check("help")) {
        printf("ColorRegionDetection options:\n");
        printf("\t--help (this help)\t--from [file.ini]\t--context [path]\n");
        printf("\t--cropSelector (default: \"%d\")\n",cropSelector);
        printf("\t--kinectDevice (device we create, default: \"%s\")\n",strKinectDevice.c_str());
        printf("\t--kinectLocal (if accesing remote, local port name, default: \"%s\")\n",strKinectLocal.c_str());
        printf("\t--kinectRemote (if accesing remote, remote port name, default: \"%s\")\n",strKinectRemote.c_str());
        printf("\t--watchdog ([s] default: \"%f\")\n",watchdog);
        // Do not exit: let last layer exit so we get help from the complete chain.
    }
    if(rf.check("cropSelector")) cropSelector = rf.find("cropSelector").asInt();
    printf("ColorRegionDetection using cropSelector: %d.\n",cropSelector);
    if(rf.check("kinectDevice")) strKinectDevice = rf.find("kinectDevice").asString();
    if(rf.check("kinectLocal")) strKinectLocal = rf.find("kinectLocal").asString();
    if(rf.check("kinectRemote")) strKinectRemote = rf.find("kinectRemote").asString();
    if(rf.check("watchdog")) watchdog = rf.find("watchdog").asDouble();
    printf("ColorRegionDetection using kinectDevice: %s, kinectLocal: %s, kinectRemote: %s.\n",
        strKinectDevice.c_str(), strKinectLocal.c_str(), strKinectRemote.c_str());
    printf("ColorRegionDetection using watchdog: %f.\n",watchdog);

    if (!rf.check("help")) {
        yarp::os::Property options;
        options.fromString( rf.toString() );  //-- Should get noMirror, noRGBMirror, noDepthMirror, video modes...
        options.put("device",strKinectDevice);  //-- Important to override in case there is a "device" in the future
        options.put("localName",strKinectLocal);  //
        options.put("remoteName",strKinectRemote);  //
        //if(rf.check("noMirror")) options.put("noMirror",1);  //-- Replaced by options.fromString( rf.toString() );
        while(!dd.open(options)) {
            printf("Waiting for kinectDevice \"%s\"...\n",strKinectDevice.c_str());
            yarp::os::Time::delay(1);
        }
        printf("[ColorRegionDetection] success: kinectDevice available.\n");
        if (! dd.view(kinect) ) fprintf(stderr,"[ColorRegionDetection] warning: kinectDevice bad view.\n");
        else printf("[ColorRegionDetection] success: kinectDevice ok view.\n");

        segmentorThread.setIKinectDeviceDriver(kinect);
        segmentorThread.setOutImg(&outImg);
        segmentorThread.setOutPort(&outPort);

        segmentorThread.setCropSelector(cropSelector);
        if(cropSelector != 0) {
            segmentorThread.setOutCropSelectorImg(&outCropSelectorImg);
            segmentorThread.setInCropSelectorPort(&inCropSelectorPort);
        }
    }

    segmentorThread.init(rf);

    //-----------------OPEN LOCAL PORTS------------//
    outImg.open(strKinectLocal + "/img:o");
    outPort.open(strKinectLocal + "/state:o");
    if(cropSelector != 0) {
        outCropSelectorImg.open(strKinectLocal + "/cropSelector/img:o");
        inCropSelectorPort.open(strKinectLocal + "/cropSelector/state:i");
    }
    return true;
}

/*****************************************************************/
double ColorRegionDetection::getPeriod() {
    return watchdog;  // [s]
}

/************************************************************************/

bool ColorRegionDetection::updateModule() {
    printf("ColorRegionDetection alive...\n");
    return true;
}

/************************************************************************/

bool ColorRegionDetection::interruptModule() {
    printf("ColorRegionDetection closing...\n");
    segmentorThread.stop();
    outImg.interrupt();
    outPort.interrupt();
    if(cropSelector != 0) {
        outCropSelectorImg.interrupt();
        inCropSelectorPort.interrupt();
    }
    dd.close();
    outImg.close();
    outPort.close();
    if(cropSelector != 0) {
        outCropSelectorImg.close();
        inCropSelectorPort.close();
    }
    return true;
}

/************************************************************************/

}  // namespace teo
