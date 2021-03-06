#include <iostream>
#include "helper/opencv_helper.h"
#include "kh_vp8.h"
#include "kinect/kinect.h"

namespace kh
{
void _display_frames()
{
    // Obtain KinectDevice to access Kinect frames.
    auto device = kinect::obtainKinectDevice();
    if (!device) {
        std::cout << "Could not find a kinect" << std::endl;
        return;
    }

    // The width and height of the Vp8Encoder are the half of Kinect frames'
    // since frames get halved before getting encoded.
    Vp8Encoder encoder(960, 540, 2000);
    Vp8Decoder decoder;

    for (;;) {
        // Try acquiring a Kinect frame until a valid frame pops up.
        auto kinect_frame = device->acquireFrame();
        if (!kinect_frame)
            continue;

        // Encodes and decodes color pixels just to test whether Vp8Encoder and Vp8Decoder works.
        // Then, converts the pixels for OpenCV.
        auto yuv_image = createHalvedYuvImageFromKinectColorBuffer(kinect_frame->color_frame()->getRawUnderlyingBuffer());
        auto vp8_frame = encoder.encode(yuv_image);
        auto ffmpeg_frame = decoder.decode(vp8_frame.data(), vp8_frame.size());
        auto color_mat = createCvMatFromYuvImage(createYuvImageFromAvFrame(ffmpeg_frame.av_frame()));

        // Compresses and decompresses the depth pixels to test the compression and decompression functions.
        // Then, converts the pixels for OpenCV.
        auto rvl_frame = createRvlFrameFromKinectDepthBuffer(kinect_frame->depth_frame()->getUnderlyingBuffer());
        auto depth_image = createDepthImageFromRvlFrame(rvl_frame.data());
        auto depth_mat = createCvMatFromKinectDepthImage(depth_image.data());

        // Displays the color and depth pixels.
        cv::imshow("Color", color_mat);
        cv::imshow("Depth", depth_mat);
        if (cv::waitKey(1) >= 0)
            break;
    }
}

void _display_intrinsics()
{
    // Obtain KinectIntrinsics using Freenect2.
    auto intrinsics = kinect::obtainKinectIntrinsics();
    if (!intrinsics) {
        std::cout << "Could not find intrinsics of a Kinect." << std::endl;
        return;
    }

    // Print all parameters of KinectIntrinsics.
    std::cout << "Color fx: " << intrinsics->color_params.fx << std::endl;
    std::cout << "Color fy: " << intrinsics->color_params.fy << std::endl;
    std::cout << "Color cx: " << intrinsics->color_params.cx << std::endl;
    std::cout << "Color cy: " << intrinsics->color_params.cy << std::endl;
    std::cout << "Color shift_d: " << intrinsics->color_params.shift_d << std::endl;
    std::cout << "Color shift_m: " << intrinsics->color_params.shift_m << std::endl;
    std::cout << "Color mx_x3y0: " << intrinsics->color_params.mx_x3y0 << std::endl;
    std::cout << "Color mx_x0y3: " << intrinsics->color_params.mx_x0y3 << std::endl;
    std::cout << "Color mx_x2y1: " << intrinsics->color_params.mx_x2y1 << std::endl;
    std::cout << "Color mx_x1y2: " << intrinsics->color_params.mx_x1y2 << std::endl;
    std::cout << "Color mx_x2y0: " << intrinsics->color_params.mx_x2y0 << std::endl;
    std::cout << "Color mx_x0y2: " << intrinsics->color_params.mx_x0y2 << std::endl;
    std::cout << "Color mx_x1y1: " << intrinsics->color_params.mx_x1y1 << std::endl;
    std::cout << "Color mx_x1y0: " << intrinsics->color_params.mx_x1y0 << std::endl;
    std::cout << "Color mx_x0y1: " << intrinsics->color_params.mx_x0y1 << std::endl;
    std::cout << "Color mx_x0y0: " << intrinsics->color_params.mx_x0y0 << std::endl;
    std::cout << "Color my_x3y0: " << intrinsics->color_params.my_x3y0 << std::endl;
    std::cout << "Color my_x0y3: " << intrinsics->color_params.my_x0y3 << std::endl;
    std::cout << "Color my_x2y1: " << intrinsics->color_params.my_x2y1 << std::endl;
    std::cout << "Color my_x1y2: " << intrinsics->color_params.my_x1y2 << std::endl;
    std::cout << "Color my_x2y0: " << intrinsics->color_params.my_x2y0 << std::endl;
    std::cout << "Color my_x0y2: " << intrinsics->color_params.my_x0y2 << std::endl;
    std::cout << "Color my_x1y1: " << intrinsics->color_params.my_x1y1 << std::endl;
    std::cout << "Color my_x1y0: " << intrinsics->color_params.my_x1y0 << std::endl;
    std::cout << "Color my_x0y1: " << intrinsics->color_params.my_x0y1 << std::endl;
    std::cout << "Color my_x0y0: " << intrinsics->color_params.my_x0y0 << std::endl;

    std::cout << "IR fx: " << intrinsics->ir_params.fx << std::endl;
    std::cout << "IR fy: " << intrinsics->ir_params.fy << std::endl;
    std::cout << "IR cx: " << intrinsics->ir_params.cx << std::endl;
    std::cout << "IR cy: " << intrinsics->ir_params.cy << std::endl;
    std::cout << "IR k1: " << intrinsics->ir_params.k1 << std::endl;
    std::cout << "IR k2: " << intrinsics->ir_params.k2 << std::endl;
    std::cout << "IR k3: " << intrinsics->ir_params.k3 << std::endl;
    std::cout << "IR p1: " << intrinsics->ir_params.p1 << std::endl;
    std::cout << "IR p2: " << intrinsics->ir_params.p2 << std::endl;
}

void display_frames()
{
    for (;;) {
        std::cout << "Press enter to display frames." << std::endl;
        std::string line;
        std::getline(std::cin, line);
        // An kind of an easter egg.
        // If "intrinsics" is entered, prints KinectIntrinsics instead of displaying frames.
        // Usually, Kinect frames are displayed.
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