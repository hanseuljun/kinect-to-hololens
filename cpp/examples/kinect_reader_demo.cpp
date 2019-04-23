#include <iostream>
#include "helper/opencv_helper.h"
#include "encoder.h"
#include "kinect/kinect.h"

namespace rgbd_streamer
{
void print_intrinsics()
{
    auto intrinsics = kinect::obtainKinectIntrinsics();

    if (intrinsics) {
        std::cout << "found kinect" << std::endl;
        std::cout << "color cx: " << intrinsics->color_params.cx << std::endl;
    } else {
        std::cout << "no kinect" << std::endl;
    }
}

void display_frames()
{
    std::cout << "start display_frames()" << std::endl;
    auto device = kinect::obtainKinectDevice();
    if (!device) {
        std::cout << "no device" << std::endl;
        return;
    }

    std::string color_name = "kinect color frame";
    std::string depth_name = "kinect depth frame";
    cv::namedWindow(color_name, 1);
    cv::namedWindow(depth_name, 1);

    for (;;) {
        auto kinect_frame = device->getFrame();
        if (!kinect_frame)
            continue;

        auto yuv420_byte_frame = createDownsampledYuv420ByteFrameFromKinectColorFrame(kinect_frame->color_frame()->getRawUnderlyingBuffer());
        auto color_bgr_mat = convertYuv420ByteFrameToBgrMat(yuv420_byte_frame);

        auto depth_bgr_mat = convertKinectDepthFrameToBgrMat(kinect_frame->depth_frame()->getUnderlyingBuffer());

        cv::imshow(color_name, color_bgr_mat);
        cv::imshow(depth_name, depth_bgr_mat);
        if (cv::waitKey(1) >= 0)
            break;
    }
}

void display_frames_with_encoding()
{
    std::cout << "start display_frames_with_encoding()" << std::endl;
    auto device = kinect::obtainKinectDevice();
    if (!device) {
        std::cout << "no device" << std::endl;
        return;
    }

    
    auto color_name = "kinect color frame";
    auto depth_name = "kinect depth frame";
    cv::namedWindow(color_name, 1);
    cv::namedWindow(depth_name, 1);

    auto encoder = createVp8Encoder(960, 540, 2000);

    for (;;) {
        auto kinect_frame = device->getFrame();
        if (!kinect_frame) {
            std::cout << "no kinect_frame" << std::endl;
            continue;
        }

        auto yuv420_byte_frame = createDownsampledYuv420ByteFrameFromKinectColorFrame(kinect_frame->color_frame()->getRawUnderlyingBuffer());
        
        auto encoded_frame = encoder->encode(yuv420_byte_frame);
        std::cout << "encoded_frame.size(): " << encoded_frame.size() << std::endl;
        
        auto color_bgr_mat = convertYuv420ByteFrameToBgrMat(yuv420_byte_frame);

        auto rvl_frame = createRvlFrameFromKinectDepthFrame(kinect_frame->depth_frame()->getUnderlyingBuffer());
        std::cout << "rvl size: " << rvl_frame.size() << std::endl;
        auto depth_frame = createDepthFrameFromRvlFrame(rvl_frame.data());

        auto underlying_buffer = kinect_frame->depth_frame()->getUnderlyingBuffer();
        for (int i = 0; i < depth_frame.size(); ++i) {
            if (depth_frame[i] != underlying_buffer[i])
                std::cout << "a descrepency at " << i << std::endl;
        }

        auto depth_bgr_mat = convertKinectDepthFrameToBgrMat(depth_frame.data());

        cv::imshow(color_name, color_bgr_mat);
        cv::imshow(depth_name, depth_bgr_mat);
        if (cv::waitKey(1) >= 0)
            break;
    }
}
}

int main()
{
    //rgbd_streamer::print_intrinsics();
    //rgbd_streamer::display_frames();
    rgbd_streamer::display_frames_with_encoding();
    std::cin.get();
    return 0;
}