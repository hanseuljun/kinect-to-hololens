#include "kinect_raii.h"	

#include <iostream>

namespace rgbd_streamer
{
namespace kinect
{
namespace raii
{
KinectColorCameraSettings::KinectColorCameraSettings(
    IColorCameraSettings* color_camera_settings)
    : color_camera_settings_(color_camera_settings)
{
}

KinectColorCameraSettings::~KinectColorCameraSettings()
{
    color_camera_settings_->Release();
}

int64_t KinectColorCameraSettings::getFrameInterval()
{
    TIMESPAN frame_interval;
    if (FAILED(color_camera_settings_->get_FrameInterval(&frame_interval)))
        throw std::exception("Failed to get frame interval in KinectColorCameraSettings::getFrameInterval...");

    return frame_interval;
}
}
}
}