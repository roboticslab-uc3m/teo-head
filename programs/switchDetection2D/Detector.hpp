// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __TRANSFORMATION_HPP__
#define __TRANSFORMATION_HPP__

#include <vector>

#include <yarp/os/all.h>
#include <yarp/sig/Image.h>

//#include "ColorRegionDetection2D.hpp"
//#include "HaarDetection2D.hpp"
//#include "TensorflowDetection2D.hpp"
//#include "TensorflowDetector.hpp"
//#include "TravisLib.hpp"

#include "ColorDebug.h"

namespace roboticslab
{

class Detector
{
public:
    Detector() : valid(false) {}
    virtual ~Detector() {}
    bool isValid() const { return valid; }
    virtual yarp::sig::ImageOf<yarp::sig::PixelRgb> detect(yarp::sig::ImageOf<yarp::sig::PixelRgb> inYarpImg) = 0;
    virtual double run(double value) { return 0.0; } // will soon perish!
protected:
    bool valid;
};

}  // namespace roboticslab

#endif // __TRANSFORMATION_HPP__
