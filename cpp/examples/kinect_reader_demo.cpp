#include <iostream>
#include "helper/opencv_helper.h"
#include "encoder.h"
#include "decoder.h"
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
        std::cout << "could not find a kinect" << std::endl;
    }
}

void display_frames()
{
    std::cout << "start display_frames()" << std::endl;
    auto device = kinect::obtainKinectDevice();
    if (!device) {
        std::cout << "could not find a kinect" << std::endl;
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

        auto yuv_frame = createHalfSizedYuvFrameFromKinectColorBuffer(kinect_frame->color_frame()->getRawUnderlyingBuffer());
        auto color_mat = convertYuvFrameToCvMat(yuv_frame);

        auto depth_mat = convertKinectDepthBufferToCvMat(kinect_frame->depth_frame()->getUnderlyingBuffer());

        cv::imshow(color_name, color_mat);
        cv::imshow(depth_name, depth_mat);
        if (cv::waitKey(1) >= 0)
            break;
    }
}

void display_frames_with_encoding()
{
    std::cout << "start display_frames_with_encoding()" << std::endl;
    auto device = kinect::obtainKinectDevice();
    if (!device) {
        std::cout << "could not find a kinect" << std::endl;
        return;
    }

    auto color_name = "kinect color frame";
    auto depth_name = "kinect depth frame";
    cv::namedWindow(color_name, 1);
    cv::namedWindow(depth_name, 1);

    auto encoder = createColorEncoder(960, 540, 2000);
    auto decoder = createColorDecoder();

    for (;;) {
        auto kinect_frame = device->getFrame();
        if (!kinect_frame)
            continue;

        auto yuv_frame = createHalfSizedYuvFrameFromKinectColorBuffer(kinect_frame->color_frame()->getRawUnderlyingBuffer());
        auto encoder_frame = encoder->encode(yuv_frame);
        auto decoder_frame = decoder->decode(encoder_frame);
        auto color_mat = convertYuvFrameToCvMat(createYuvFrameFromAvFrame(decoder_frame.av_frame()));

        auto rvl_frame = createRvlFrameFromKinectDepthBuffer(kinect_frame->depth_frame()->getUnderlyingBuffer());
        auto depth_frame = createDepthFrameFromRvlFrame(rvl_frame.data());
        auto depth_mat = convertKinectDepthBufferToCvMat(depth_frame.data());

        cv::imshow(color_name, color_mat);
        cv::imshow(depth_name, depth_mat);
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
    //rgbd_streamer::display_textures();
    std::cin.get();
    return 0;
}