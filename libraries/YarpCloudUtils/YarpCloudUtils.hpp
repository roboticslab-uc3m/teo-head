// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __YARP_CLOUD_UTILS_HPP__
#define __YARP_CLOUD_UTILS_HPP__

#include <string>

#include <yarp/os/Property.h>
#include <yarp/os/Searchable.h>

#include <yarp/sig/PointCloud.h>
#include <yarp/sig/Vector.h>

/**
 * @ingroup vision_libraries
 * @defgroup YarpCloudUtils
 */

namespace roboticslab
{

namespace YarpCloudUtils
{

template <typename T>
bool savePLY(const std::string & filename, const yarp::sig::PointCloud<T> & cloud, const yarp::sig::VectorOf<int> & indices, bool isBinary = true);

template <typename T>
bool savePLY(const std::string & filename, const yarp::sig::PointCloud<T> & cloud, bool isBinary = true)
{
    return savePLY(filename, cloud, {}, isBinary);
}

template <typename T>
bool loadPLY(const std::string & filename, yarp::sig::PointCloud<T> & cloud, yarp::sig::VectorOf<int> & indices);

template <typename T>
bool loadPLY(const std::string & filename, yarp::sig::PointCloud<T> & cloud)
{
    yarp::sig::VectorOf<int> indices;
    return loadPLY(filename, cloud, indices);
}

template <typename T1, typename T2 = T1>
bool meshFromCloud(const yarp::sig::PointCloud<T1> & cloud,
                   yarp::sig::PointCloud<T2> & meshPoints,
                   yarp::sig::VectorOf<int> & meshIndices,
                   const yarp::sig::VectorOf<yarp::os::Property> & options = {
                       {{"algorithm", yarp::os::Value("VoxelGrid")},
                        {"leafSize", yarp::os::Value(0.02f)}},
                       {{"algorithm", yarp::os::Value("NormalEstimationOMP")},
                        {"kSearch", yarp::os::Value(40)}},
                       {{"algorithm", yarp::os::Value("Poisson")}}
                   });

template <typename T1, typename T2 = T1>
bool meshFromCloud(const yarp::sig::PointCloud<T1> & cloud,
                   yarp::sig::PointCloud<T2> & meshPoints,
                   yarp::sig::VectorOf<int> & meshIndices,
                   const yarp::os::Searchable & config,
                   const std::string & collection = "meshPipeline");

} // namespace YarpCloudUtils

} // namespace roboticslab

#endif // __YARP_CLOUD_UTILS_HPP__
