#include "kinect.h"	

#include <iostream>

namespace rgbd_streamer
{
namespace kinect
{
KinectDevice::KinectDevice(std::unique_ptr<raii::KinectSensor> sensor,
    std::unique_ptr<raii::KinectMultiSourceFrameReader> frame_reader)
    : sensor_(std::move(sensor)), frame_reader_(std::move(frame_reader))
{
}

bool KinectDevice::isAvailable()
{
    return sensor_->isAvailable();
}

std::unique_ptr<KinectFrame> KinectDevice::getFrame()
{
    auto multi_source_frame = frame_reader_->getMultiSourceFrame();
    // This can mean Kinect having a problem, or simply not having a new frame yet.
    if (!multi_source_frame) {
        return nullptr;
    }

    auto color_frame = multi_source_frame->getColorFrame();
    if (!color_frame) {
        std::cout << "color_frame not found in KinectDevice::getFrame..." << std::endl;
        return nullptr;
    }

    auto depth_frame = multi_source_frame->getDepthFrame();
    if (!depth_frame) {
        std::cout << "depth_frame not found in KinectDevice::getFrame..." << std::endl;
        return nullptr;
    }

    auto body_frame = multi_source_frame->getBodyFrame();
    if (!body_frame) {
        std::cout << "body_frame not found in KinectDevice::getFrame..." << std::endl;
        return nullptr;
    }

    return std::make_unique<KinectFrame>(
        std::move(color_frame), std::move(depth_frame), std::move(body_frame));
}

std::unique_ptr<KinectDevice> obtainKinectDevice()
{
    auto sensor = raii::KinectSensor::create();
    if (!sensor)
        return nullptr;

    auto frame_reader = sensor->getMultiSourceFrameReader();
    if (!frame_reader)
        return nullptr;

    return std::make_unique<KinectDevice>(std::move(sensor), std::move(frame_reader));
}

std::optional<KinectIntrinsics> obtainKinectIntrinsics()
{
    libfreenect2::Freenect2 freenect2;
    libfreenect2::Freenect2Device* device;

    // Don't know why, but at the first attempt to open the device,
    // it fails with an LIBSUB_ERROR_OTHER when the Kinect is already
    // connected to the laptop through the official API.
    for (int i = 0; i < 2; ++i) {
        device = freenect2.openDefaultDevice();
        if (device)
            break;
    }

    if (!device)
        return std::nullopt;
    if (!device->start())
        return std::nullopt;

    KinectIntrinsics intrinsics;
    intrinsics.color_params = device->getColorCameraParams();
    intrinsics.ir_params = device->getIrCameraParams();

    if (!device->stop())
        return std::nullopt;
    if (!device->close())
        return std::nullopt;

    return intrinsics;
}
}
}