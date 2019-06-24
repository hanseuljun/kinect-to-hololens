#include "kinect_raii.h"	

#include <iostream>

namespace kh
{
namespace kinect
{
namespace raii
{
// An RAII class for IColorFrame.
KinectColorFrame::KinectColorFrame(IColorFrame* color_frame)
    : color_frame_(color_frame)
{
}

KinectColorFrame::~KinectColorFrame()
{
    color_frame_->Release();
}

std::unique_ptr<KinectColorCameraSettings> KinectColorFrame::getColorCameraSettings()
{
    IColorCameraSettings* color_camera_settings = nullptr;
    if (FAILED(color_frame_->get_ColorCameraSettings(&color_camera_settings))) {
        std::cout << "Failed to get an IColorCameraSettings in KinectColorFrame::getColorCameraSettings..." << std::endl;
        return nullptr;
    }

    return std::make_unique<KinectColorCameraSettings>(color_camera_settings);
}

BYTE* KinectColorFrame::getRawUnderlyingBuffer()
{
    UINT buffer_size = 0;
    BYTE* buffer = nullptr;

    if (FAILED(color_frame_->AccessRawUnderlyingBuffer(&buffer_size, &buffer))) {
        std::cout << "Failed in KinectColorFrame::getRawUnderlyingBuffer..." << std::endl;
        return nullptr;
    }

    return buffer;
}
}
}
}