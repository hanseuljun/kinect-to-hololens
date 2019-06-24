#include "kinect_raii.h"	

#include <iostream>

namespace kh
{
namespace kinect
{
namespace raii
{
// An RAII class for IMultiSourceFrameReader.
KinectMultiSourceFrameReader::KinectMultiSourceFrameReader(
    IMultiSourceFrameReader* multi_source_frame_reader)
    : multi_source_frame_reader_(multi_source_frame_reader)
{
}

KinectMultiSourceFrameReader::~KinectMultiSourceFrameReader()
{
    multi_source_frame_reader_->Release();
}

std::unique_ptr<KinectMultiSourceFrame> KinectMultiSourceFrameReader::getMultiSourceFrame()
{
    IMultiSourceFrame* multi_source_frame = nullptr;
    if (FAILED(multi_source_frame_reader_->AcquireLatestFrame(&multi_source_frame)))
        return nullptr;

    return std::make_unique<KinectMultiSourceFrame>(multi_source_frame);
}
}
}
}