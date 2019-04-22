#include "kinect_raii.h"	

#include <iostream>

namespace rgbd_streamer
{
namespace kinect
{
namespace raii
{
KinectDepthFrame::KinectDepthFrame(IDepthFrame* depth_frame)
	: depth_frame_(depth_frame)
{
}

KinectDepthFrame::~KinectDepthFrame()
{
	depth_frame_->Release();
}

UINT16* KinectDepthFrame::getBuffer()
{
	UINT buffer_size = 0;
	UINT16* buffer = 0;
	HRESULT hr = depth_frame_->AccessUnderlyingBuffer(&buffer_size, &buffer);

	return buffer;
}
}
}
}