// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/os/Bottle.h>
#include <yarp/sig/ImageDraw.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <ColorDebug.h>

#include "ColorRegionDetector.hpp"

namespace roboticslab
{

/*****************************************************************/

ColorRegionDetector::ColorRegionDetector(yarp::os::Searchable* parameters)
{
    algorithm = DEFAULT_ALGORITHM;
    CD_DEBUG("*** \"algorithm\" (default: \"%s\")\n", algorithm.c_str());
    if(parameters->check("algorithm"))
    {
        CD_INFO("**** \"algorithm\" parameter for ColorRegionDetectionTransformation found\n");
        algorithm = parameters->find("algorithm").asString();
    }
    CD_DEBUG("ColorRegionDetector using algorithm: %s.\n", algorithm.c_str());

    maxNumBlobs = DEFAULT_MAX_NUM_BLOBS;
    if(parameters->check("maxNumBlobs"))
    {
        CD_INFO("**** \"maxNumBlobs\" parameter for ColorRegionDetectionTransformation found\n");
        maxNumBlobs = parameters->find("maxNumBlobs").asInt32();
    }

    morphClosing = DEFAULT_MORPH_CLOSING;
    if(parameters->check("morphClosing"))
    {
        CD_INFO("**** \"morphClosing\" parameter for ColorRegionDetectionTransformation found\n");
        morphClosing = parameters->find("morphClosing").asFloat64();
    }
    CD_DEBUG("ColorRegionDetector using morphClosing: %f.\n", morphClosing);

    threshold = DEFAULT_THRESHOLD;
    if(parameters->check("threshold"))
    {
        CD_INFO("**** \"threshold\" parameter for ColorRegionDetectionTransformation found\n");
        threshold = parameters->find("threshold").asInt32();
    }
    CD_DEBUG("ColorRegionDetector using threshold: %d.\n", threshold);

    valid = true;
}

/*****************************************************************/

bool ColorRegionDetector::detect(yarp::sig::ImageOf<yarp::sig::PixelRgb> inYarpImg,
                                 std::vector<DetectedObject*>& detectedObjects,
                                 yarp::sig::ImageOf<yarp::sig::PixelRgb>& ret)
{

    // {yarp ImageOf Rgb -> openCv Mat Bgr}
    IplImage *inIplImage = cvCreateImage(cvSize(inYarpImg.width(), inYarpImg.height()),
                                         IPL_DEPTH_8U, 3 );
    cvCvtColor((IplImage*)inYarpImg.getIplImage(), inIplImage, CV_RGB2BGR);
    cv::Mat inCvMat = cv::cvarrToMat(inIplImage);

    // Because Travis stuff goes with [openCv Mat Bgr] for now
    Travis travis(false,true);    // ::Travis(quiet=true, overwrite=true);
    travis.setCvMat(inCvMat);
    if (algorithm=="hue")
        travis.binarize("hue", threshold-5,threshold+5);
    else if(algorithm=="canny")
        travis.binarize("canny");
    else
        travis.binarize(algorithm.c_str(), threshold);
    travis.morphClosing(inYarpImg.width() * morphClosing / 100.0 );
    int numBlobs = travis.blobize(maxNumBlobs);
    if( 0 == numBlobs )
    {
        travis.release();
        return false;
    }
    std::vector<cv::Point2d> blobsXY;
    if( ! travis.getBlobsXY(blobsXY) )
    {
        travis.release();
        return false;
    }

    travis.release();

    return true;
}

/************************************************************************/

}  // namespace roboticslab
