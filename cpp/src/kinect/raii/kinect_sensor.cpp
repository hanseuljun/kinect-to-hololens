#include "kinect_raii.h"	

#include <iostream>

namespace kh
{
namespace kinect
{
namespace raii
{
KinectSensor::KinectSensor(IKinectSensor* kinect_sensor)
    : kinect_sensor_(kinect_sensor)
{
}

KinectSensor::~KinectSensor()
{
    kinect_sensor_->Close();
}

std::unique_ptr<KinectSensor> KinectSensor::create()
{
    IKinectSensor* kinect_sensor = nullptr;
    if (FAILED(GetDefaultKinectSensor(&kinect_sensor))) {
        std::cout << "GetDefaultKinectSensor in KinectSensor::create failed..." << std::endl;
        return nullptr;
    }

    if (!kinect_sensor) {
        std::cout << "kinect_sensor is null in KinectSensor::create..." << std::endl;
        return nullptr;
    }

    if (FAILED((kinect_sensor)->Open())) {
        std::cout << "Failed to open kinect_sensor in KinectSensor::create..." << std::endl;
        return nullptr;
    }

    return std::make_unique<KinectSensor>(kinect_sensor);
}

bool KinectSensor::isAvailable()
{
    BOOLEAN is_available;
    if (FAILED(kinect_sensor_->get_IsAvailable(&is_available)))
        throw new std::exception("KinectSensor::isAvailable failed...");

    return is_available > 0 ? true : false;
}

std::unique_ptr<KinectMultiSourceFrameReader> KinectSensor::getMultiSourceFrameReader()
{
    IMultiSourceFrameReader* multi_source_frame_reader = nullptr;
    DWORD frame_source_types = FrameSourceTypes_Color | FrameSourceTypes_Depth;
    if (FAILED((kinect_sensor_)->OpenMultiSourceFrameReader(frame_source_types, &multi_source_frame_reader))) {
        std::cout << "KinectSensor::getMultiSourceFrameReader failed..." << std::endl;
        return nullptr;
    }

    return std::make_unique<KinectMultiSourceFrameReader>(multi_source_frame_reader);
}
}
}
}