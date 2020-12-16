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

#include "IDetector.hpp"

#define DEFAULT_RATE_MS 20

namespace roboticslab
{

/**
 * @ingroup rgbdDetection
 *
 * @brief Implements rgbdDetection callback on Bottle.
 */
class DataProcessor : public yarp::os::PortReader
{
    virtual bool read(yarp::os::ConnectionReader& connection)
    {
        yarp::os::Bottle b;
        b.read(connection);
        // process data in b
        printf("Got %s\n", b.toString().c_str());

        if (waitForFirst)
        {
            xKeep = b.get(0).asInt32();
            yKeep = b.get(1).asInt32();
            waitForFirst = false;
        }
        else
        {
            if (b.get(0).asInt32() < xKeep || b.get(1).asInt32() < yKeep)
            {
                x = 0;
                y = 0;
                w = 0;
                h = 0;
            }
            else
            {
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
    void reset()
    {
        waitForFirst = true;
        x = y = w = h = 0;
        xKeep = yKeep = 0;
    }

    int xKeep, yKeep;
    int x, y, w, h;
    bool waitForFirst;
};

/**
 * @ingroup rgbdDetection
 *
 * @brief Implements rgbdDetection PeriodicThread.
 */
class SegmentorThread : public yarp::os::PeriodicThread
{
public:
    SegmentorThread() : PeriodicThread(DEFAULT_RATE_MS * 0.001) {}

    void setIRGBDSensor(yarp::dev::IRGBDSensor * _iRGBDSensor);
    void setOutImg(yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb> > * _pOutImg);
    void setOutPort(yarp::os::Port *_pOutPort);
    bool init(yarp::os::ResourceFinder &rf);

    void setCropSelector(int cropSelector) { this->cropSelector = cropSelector; }
    void setOutCropSelectorImg(yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb> >* outCropSelectorImg) { this->outCropSelectorImg = outCropSelectorImg; }
    void setInCropSelectorPort(yarp::os::Port* inCropSelectorPort) { this->inCropSelectorPort = inCropSelectorPort; }

private:
    void run() override; // The periodical function

    yarp::dev::PolyDriver detectorDevice;
    IDetector* iDetector;

    yarp::dev::IRGBDSensor *iRGBDSensor;
    yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb> > *pOutImg;  // for testing
    yarp::os::Port *pOutPort;
    //
    double fx_d,fy_d,cx_d,cy_d;
    //
    int cropSelector;
    yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb> >* outCropSelectorImg;
    yarp::os::Port* inCropSelectorPort;
    DataProcessor processor;
};

}  // namespace roboticslab

#endif  // __SEGMENTOR_THREAD_HPP__