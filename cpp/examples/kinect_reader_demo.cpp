#include <iostream>
#include "helper/opencv_helper.h"
#include "kh_vp8.h"
#include "kinect/kinect.h"

namespace kh
{
void _display_frames()
{
    auto device = kinect::obtainKinectDevice();
    if (!device) {
        std::cout << "Could not find a kinect" << std::endl;
        return;
    }

    Vp8Encoder encoder(960, 540, 2000);
    Vp8Decoder decoder;

    for (;;) {
        auto kinect_frame = device->acquireFrame();
        if (!kinect_frame)
            continue;

        auto yuv_image = createHalvedYuvImageFromKinectColorBuffer(kinect_frame->color_frame()->getRawUnderlyingBuffer());
        auto vp8_frame = encoder.encode(yuv_image);
        auto ffmpeg_frame = decoder.decode(vp8_frame);
        auto color_mat = createCvMatFromYuvImage(createYuvImageFromAvFrame(ffmpeg_frame.av_frame()));

        auto rvl_frame = createRvlFrameFromKinectDepthBuffer(kinect_frame->depth_frame()->getUnderlyingBuffer());
        auto depth_image = createDepthImageFromRvlFrame(rvl_frame.data());
        auto depth_mat = createCvMatFromKinectDepthImage(depth_image.data());

        cv::imshow("Color", color_mat);
        cv::imshow("Depth", depth_mat);
        if (cv::waitKey(1) >= 0)
            break;
    }
}

void _display_intrinsics()
{
    auto intrinsics = kinect::obtainKinectIntrinsics();
    if (!intrinsics) {
        std::cout << "Could not find intrinsics of a Kinect." << std::endl;
        return;
    }

    std::cout << "Color fx: " << intrinsics->color_params.fx << std::endl;
    std::cout << "Color fy: " << intrinsics->color_params.fy << std::endl;
    std::cout << "Color cx: " << intrinsics->color_params.cx << std::endl;
    std::cout << "Color cy: " << intrinsics->color_params.cy << std::endl;

    std::cout << "IR fx: " << intrinsics->ir_params.fx << std::endl;
    std::cout << "IR fy: " << intrinsics->ir_params.fy << std::endl;
    std::cout << "IR cx: " << intrinsics->ir_params.cx << std::endl;
    std::cout << "IR cy: " << intrinsics->ir_params.cy << std::endl;
}

void display_frames()
{
    for (;;) {
        std::cout << "Press enter display frames." << std::endl;
        std::string line;
        std::getline(std::cin, line);
        if (line == "intrinsics") {
            _display_intrinsics();
        } else {
            _display_frames();
        }
    }
}
}

int main()
{
    kh::display_frames();
    return 0;
}