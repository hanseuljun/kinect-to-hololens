#include "kinect.h"	

#include <iostream>

namespace kh
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

// Trys acquiring a frame. Returns nullptr if there is a problem obtaining the frame.
std::unique_ptr<KinectFrame> KinectDevice::acquireFrame()
{
    auto multi_source_frame = frame_reader_->getMultiSourceFrame();
    // This can mean Kinect having a problem, or simply not having a new frame yet.
    if (!multi_source_frame)
        return nullptr;

    auto color_frame = multi_source_frame->getColorFrame();
    if (!color_frame)
        return nullptr;

    auto depth_frame = multi_source_frame->getDepthFrame();
    if (!depth_frame)
        return nullptr;

    return std::make_unique<KinectFrame>(std::move(color_frame), std::move(depth_frame));
}

// Trys acquiring a KinectDevice. Returns nullptr if there is a problem obtaining the frame.
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

// Trys acquiring a KinectIntrinsics. Returns nullptr if there is a problem obtaining the frame.
std::optional<KinectIntrinsics> obtainKinectIntrinsics()
{
    libfreenect2::Freenect2 freenect2;
    libfreenect2::Freenect2Device* device;

    // Don't know why, but at the first attempt to open the device,
    // Freenect2 fails with an LIBSUB_ERROR_OTHER.
    for (int i = 0; i < 2; ++i) {
        device = freenect2.openDefaultDevice();
        if (device)
            break;
    }

    if (!device) {
        std::cout << "Check whether UsbDk has been installed to this machine." << std::endl;
        return std::nullopt;
    }

    if (!device->start()) {
        std::cout << "Failed to start Freenect2 device." << std::endl;
        return std::nullopt;
    }

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