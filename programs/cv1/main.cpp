// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/**
 *
 * @ingroup teo_head_programs
 *
 * \defgroup cv1 cv1
 *
 * @brief Creates an instance ---- of teo::Cv1.
 *
 * @section cv1_options Cv1 options:
 *
 * |PROPERTY     | DESCRIPTION                           | DEFAULT              |
 * |-------------|---------------------------------------|----------------------|
 * |help         |                                       |                      |
 * |from         |file.ini                               |                      |
 * |context      |path                                   |                      |
 * |cropSelector |                                       | 0                    |
 * |kinectDevice |device we create                       | OpenNI2DeviceServer  |
 * |kinectLocal  |if accesing remote, local port name    | /cv1                 |
 * |kinectRemote |if accesing remote, remote port name   | /OpenNI2             |
 * |watchdog     |                                       | 2.000000             |
 *
 *
 * @section segmentorthread_options SegmentorThread options:
 *
 * |PROPERTY             | DESCRIPTION                   | DEFAULT              |
 * |---------------------|-------------------------------|----------------------|
 * |help                 |                               |                      |
 * |from                 |file.ini                       |                      |
 * |context              |path                           |                      |
 * |fx_d                 |                               |525.000000            |
 * |fy_d                 |                               |525.000000            |
 * |cx_d                 |                               |319.500000            |
 * |cy_d                 |                               |239.500000            |
 * |fx_rgb               |                               |525.000000            |
 * |fy_rgb               |                               |525.000000            |
 * |cx_rgb               |                               |319.500000            |
 * |cy_rgb               |                               |239.500000            |
 * |algorithm            |                               |blueMinusRed          |
 * |locate               |centroid or bottom             |centroid              |
 * |maxNumBlobs          |                               |2                     |
 * |morphClosing         |percentage, 2 or 4 okay        |2.000000              |
 * |morphOpening         |percentage, 2 or 4 okay        |0.000000              |
 * |outFeatures          |mmX, mmY, mmZ, pxXpos, pxYpos, pxX, pxY, angle, area, aspectRatio, rectangularity, axisFirst, axisSecond solidity, hue, sat, val, hueStdDev, satStdDev, valStdDev, time |(mmX mmY mmZ)
 * |outFeaturesFormat    |0=bottled,1=minimal            |0                     |
 * |outImage             |0=rgb,1=bin                    |1                     |
 * |rateMs               |                               |20                    |
 * |seeBounding          |0=none,1=box,2=contour,3=both  |3                     |
 * |threshold            |                               |55                    |
 *
 *
 */

#include "Cv1.hpp"

int main(int argc, char** argv) {

    yarp::os::ResourceFinder rf;
    rf.setVerbose(true);
    rf.setDefaultContext("cv1");
    rf.setDefaultConfigFile("cv1.ini");
    rf.configure(argc, argv);

    teo::Cv1 mod;
    if(rf.check("help")) {
        return mod.runModule(rf);
    }

    printf("Run \"%s --help\" for options.\n",argv[0]);
    printf("%s checking for yarp network... ",argv[0]);
    fflush(stdout);
    yarp::os::Network yarp;
    if (!yarp.checkNetwork()) {
        fprintf(stderr,"[fail]\n%s found no yarp network (try running \"yarpserver &\"), bye!\n",argv[0]);
        return -1;
    } else printf("[ok]\n");

    return mod.runModule(rf);
}

