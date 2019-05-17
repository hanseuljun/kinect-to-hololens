#include "kinect_raii.h"	

#include <iostream>

namespace kh
{
namespace kinect
{
namespace raii
{
KinectMultiSourceFrame::KinectMultiSourceFrame(IMultiSourceFrame* multi_source_frame)
    : multi_source_frame_(multi_source_frame)
{
}

KinectMultiSourceFrame::~KinectMultiSourceFrame()
{
    multi_source_frame_->Release();
}

std::unique_ptr<KinectColorFrame> KinectMultiSourceFrame::getColorFrame()
{
    IColorFrameReference* color_frame_reference = nullptr;
    if (FAILED(multi_source_frame_->get_ColorFrameReference(&color_frame_reference)))
        return nullptr;

    IColorFrame* color_frame = nullptr;
    if (FAILED(color_frame_reference->AcquireFrame(&color_frame)))
        return nullptr;

    return std::make_unique<KinectColorFrame>(color_frame);
}

std::unique_ptr<KinectDepthFrame> KinectMultiSourceFrame::getDepthFrame()
{
    IDepthFrameReference* depth_frame_reference = nullptr;
    if (FAILED(multi_source_frame_->get_DepthFrameReference(&depth_frame_reference)))
        return nullptr;

    IDepthFrame* depth_frame = nullptr;
    if (FAILED(depth_frame_reference->AcquireFrame(&depth_frame)))
        return nullptr;

    return std::make_unique<KinectDepthFrame>(depth_frame);
}
}
}
}