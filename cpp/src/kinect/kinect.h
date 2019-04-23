#pragma once

#include <optional>
#include "libfreenect2/libfreenect2.hpp"
#include "raii/kinect_raii.h"

namespace rgbd_streamer
{
namespace kinect
{
class KinectFrame
{
public:
    KinectFrame(std::shared_ptr<raii::KinectColorFrame> color_frame,
        std::shared_ptr<raii::KinectDepthFrame> depth_frame,
        std::shared_ptr<raii::KinectBodyFrame> body_frame)
        : color_frame_(color_frame), depth_frame_(depth_frame), body_frame_(body_frame)
    {
    }
    std::shared_ptr<raii::KinectColorFrame> color_frame() { return color_frame_; }
    std::shared_ptr<raii::KinectDepthFrame> depth_frame() { return depth_frame_; }
    std::shared_ptr<raii::KinectBodyFrame> body_frame() { return body_frame_; }

private:
    std::shared_ptr<raii::KinectColorFrame> color_frame_;
    std::shared_ptr<raii::KinectDepthFrame> depth_frame_;
    std::shared_ptr<raii::KinectBodyFrame> body_frame_;
};

class KinectDevice
{
public:
    KinectDevice(std::unique_ptr<raii::KinectSensor> sensor,
        std::unique_ptr<raii::KinectMultiSourceFrameReader> frame_reader);
    bool isAvailable();
    std::unique_ptr<KinectFrame> KinectDevice::getFrame();

private:
    std::unique_ptr<raii::KinectSensor> sensor_;
    std::unique_ptr<raii::KinectMultiSourceFrameReader> frame_reader_;
};

struct KinectIntrinsics
{
    libfreenect2::Freenect2Device::ColorCameraParams color_params;
    libfreenect2::Freenect2Device::IrCameraParams ir_params;
};

// Utility functions.
std::unique_ptr<KinectDevice> obtainKinectDevice();
std::optional<KinectIntrinsics> obtainKinectIntrinsics();
}
}