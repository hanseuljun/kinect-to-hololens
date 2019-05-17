#include <iostream>
#include "helper/opencv_helper.h"
#include "vp8.h"
#include "kinect/kinect.h"

namespace kh
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
        auto kinect_frame = device->acquireFrame();
        if (!kinect_frame)
            continue;

        auto yuv_frame = createHalfSizedYuvImageFromKinectColorBuffer(kinect_frame->color_frame()->getRawUnderlyingBuffer());
        auto color_mat = createCvMatFromYuvImage(yuv_frame);

        auto depth_mat = createCvMatFromKinectDepthBuffer(kinect_frame->depth_frame()->getUnderlyingBuffer());

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

    Vp8Encoder encoder(960, 540, 2000);
    Vp8Decoder decoder;

    for (;;) {
        auto kinect_frame = device->acquireFrame();
        if (!kinect_frame)
            continue;

        auto yuv_image = createHalfSizedYuvImageFromKinectColorBuffer(kinect_frame->color_frame()->getRawUnderlyingBuffer());
        auto encoder_frame = encoder.encode(yuv_image);
        auto decoder_frame = decoder.decode(encoder_frame);
        auto color_mat = createCvMatFromYuvImage(createYuvImageFromAvFrame(decoder_frame.av_frame()));

        auto rvl_frame = createRvlFrameFromKinectDepthBuffer(kinect_frame->depth_frame()->getUnderlyingBuffer());
        auto depth_frame = createDepthFrameFromRvlFrame(rvl_frame.data());
        auto depth_mat = createCvMatFromKinectDepthBuffer(depth_frame.data());

        cv::imshow(color_name, color_mat);
        cv::imshow(depth_name, depth_mat);
        if (cv::waitKey(1) >= 0)
            break;
    }
}
}

int main()
{
    //kh::print_intrinsics();
    //kh::display_frames();
    kh::display_frames_with_encoding();
    std::cin.get();
    return 0;
}