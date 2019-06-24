#include "kinect_raii.h"	

#include <iostream>

namespace kh
{
namespace kinect
{
namespace raii
{
// An RAII class for IDepthFrame.
KinectDepthFrame::KinectDepthFrame(IDepthFrame* depth_frame)
    : depth_frame_(depth_frame)
{
}

KinectDepthFrame::~KinectDepthFrame()
{
    depth_frame_->Release();
}

UINT16* KinectDepthFrame::getUnderlyingBuffer()
{
    UINT buffer_size = 0;
    UINT16* buffer = nullptr;

    if (FAILED(depth_frame_->AccessUnderlyingBuffer(&buffer_size, &buffer))) {
        std::cout << "Failed in KinectDepthFrame::getUnderlyingBuffer..." << std::endl;
        return nullptr;
    }

    return buffer;
}
}
}
}