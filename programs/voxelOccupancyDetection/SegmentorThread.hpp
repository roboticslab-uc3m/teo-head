// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __SEGMENTOR_THREAD_HPP__
#define __SEGMENTOR_THREAD_HPP__

#include <yarp/os/RFModule.h>
#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Property.h>

#include <yarp/dev/all.h>
#include <yarp/dev/IRGBDSensor.h>

#include <yarp/sig/all.h>

#define DEFAULT_ALGORITHM "blueMinusRed"
#define DEFAULT_LOCATE "centroid"
#define DEFAULT_MAX_NUM_BLOBS 2
#define DEFAULT_MORPH_CLOSING 2
#define DEFAULT_MORPH_OPENING 0
#define DEFAULT_OUT_FEATURES "mmX mmY mmZ"  // it's a bottle!!
#define DEFAULT_OUT_FEATURES_FORMAT 0  // 0=bottled,1=minimal
#define DEFAULT_OUT_IMAGE 1
#define DEFAULT_RATE_MS 20
#define DEFAULT_SEE_BOUNDING 3
#define DEFAULT_THRESHOLD 55

//VoxelOccupancy Constants
#define DEFAULT_SEARCH_AREA_DILATATION 10
#define DEFAULT_DEPTH_LOW_THRESHOLD 0.730 //m; TV Dimensions
#define DEFAULT_DEPTH_HIGH_THRESHOLD 1.25 //m; TV Dimensions
#define DEFAULT_OCCUPANCY_THRESHOLD 100
#define DEFAULT_VOXEL_RESOLUTION 4 //this is the number of voxel per row.
#define DEFAULT_UTILITY_DEPTH_LOW_THRESHOLD 1.350 //m;
#define DEFAULT_UTILITY_DEPTH_HIGH_THRESHOLD 1.450 //mm;
#define DEFAULT_NUMBER_UTILITY_VOXELS 4
// W and H threshold are as a function of the total value.
#define DEFAULT_LOW_W_THRESHOLD 0.17
#define DEFAULT_HIGH_W_THRESHOLD 0.87
#define DEFAULT_LOW_H_THRESHOLD 0.45
#define DEFAULT_HIGH_H_THRESHOLD 0.52


namespace roboticslab
{

/**
 * @ingroup voxelOccupancyDetection
 *
 * @brief Implements voxelOccupancyDetection callback on Bottle.
 */
class CropSelectorDataProcessor : public yarp::os::PortReader {
    virtual bool read(yarp::os::ConnectionReader& connection) {
        yarp::os::Bottle b;
        b.read(connection);
        // process data in b
        printf("Got %s\n", b.toString().c_str());
        if(waitForFirst) {
            xKeep = b.get(0).asInt32();
            yKeep = b.get(1).asInt32();
            waitForFirst = false;
        } else {
            if((b.get(0).asInt32()<xKeep)||(b.get(1).asInt32()<yKeep)){
                x = 0;
                y = 0;
                w = 0;
                h = 0;
            } else {
                x = xKeep;
                y = yKeep;
                w = b.get(0).asInt32() - x;
                h = b.get(1).asInt32() - y;
            }
            waitForFirst = true;
        }
        return true;

    }
public:
    bool reset() {
        waitForFirst = true;
        x = 0;
        y = 0;
        w = 0;
        h = 0;
        xKeep = 0;
        yKeep = 0;
    }
    int xKeep, yKeep;
    int x, y, w, h;
    bool waitForFirst;
};

/**
 * @ingroup voxelOccupancyDetection
 *
 * @brief Implements voxelOccupancyDetection PeriodicThread.
 */
class SegmentorThread : public yarp::os::PeriodicThread {
private:
    yarp::dev::IRGBDSensor *iRGBDSensor;
    yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelMono16> > *pOutImg;  // for testing
    yarp::os::Port *pOutPort;
    //
    std::string algorithm;
    std::string locate;
    int maxNumBlobs;
    double morphClosing;
    double morphOpening;
    int outFeaturesFormat;
    int outImage;
    int seeBounding;
    int threshold;
    //
    double fx_d,fy_d,cx_d,cy_d,fx_rgb,fy_rgb,cx_rgb,cy_rgb;
    //
    yarp::os::Bottle outFeatures;
    //
    int cropSelector;
    yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb> >* outCropSelectorImg;
    yarp::os::Port* inCropSelectorPort;
    CropSelectorDataProcessor processor;

    //VoxelOccupancy specific variables
    int searchAreaDilatation;
    float depthLowThreshold;
    float depthHighThreshold;
    int occupancyThreshold;
    int voxelResolution;
    float utilityDepthLowThreshold;
    float utilityDepthHighThreshold;
    int numberUtilityVoxels;
    float lowWThreshold;
    float highWThreshold;
    float lowHThreshold;
    float highHThreshold;
public:
    SegmentorThread() : PeriodicThread(DEFAULT_RATE_MS * 0.001) {}

    void setIRGBDSensor(yarp::dev::IRGBDSensor * _iRGBDSensor);
    void setOutImg(yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelMono16> > * _pOutImg);
    void setOutPort(yarp::os::Port *_pOutPort);
    void init(yarp::os::ResourceFinder &rf);
    void run();  // The periodical function

    void setCropSelector(int cropSelector) { this->cropSelector = cropSelector; }
    void setOutCropSelectorImg(yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb> >* outCropSelectorImg) { this->outCropSelectorImg = outCropSelectorImg; }
    void setInCropSelectorPort(yarp::os::Port* inCropSelectorPort) { this->inCropSelectorPort = inCropSelectorPort; }

};

}  // namespace roboticslab

#endif  // __SEGMENTOR_THREAD_HPP__

