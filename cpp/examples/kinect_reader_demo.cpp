#include <iostream>
#include <Windows.h>
#include <opencv2/opencv.hpp>
#include "frames.h"
#include "kinect/kinect.h"
#include "helper/opencv_helper.h"

namespace rgbd_streamer
{
void print_intrinsics()
{
    auto intrinsics = kinect::obtainKinectIntrinsics();

    if (intrinsics)
    {
        std::cout << "found kinect" << std::endl;

        // Commented numbers are of the lab Kinect I'm now using.
        std::cout << "-----color_camera_params-----" << std::endl;
        std::cout << "cx: " << intrinsics->color_params.cx << std::endl; // 959.5
        std::cout << "cy: " << intrinsics->color_params.cy << std::endl; // 539.5
        std::cout << "fx: " << intrinsics->color_params.fx << std::endl; // 1081.37
        std::cout << "fy: " << intrinsics->color_params.fy << std::endl; // 1081.37
        std::cout << "shift_d: " << intrinsics->color_params.shift_d << std::endl; // 863
        std::cout << "shift_m: " << intrinsics->color_params.shift_m << std::endl; // 52
        std::cout << "mx_x3y0: " << intrinsics->color_params.mx_x3y0 << std::endl; // xxx // 0.000342423
        std::cout << "mx_x0y3: " << intrinsics->color_params.mx_x0y3 << std::endl; // yyy // -1.72204e-05
        std::cout << "mx_x2y1: " << intrinsics->color_params.mx_x2y1 << std::endl; // xxy // 2.51542e-05
        std::cout << "mx_x1y2: " << intrinsics->color_params.mx_x1y2 << std::endl; // yyx // 0.000180899
        std::cout << "mx_x2y0: " << intrinsics->color_params.mx_x2y0 << std::endl; // xx // 0.000417287
        std::cout << "mx_x0y2: " << intrinsics->color_params.mx_x0y2 << std::endl; // yy // -6.43967e-05
        std::cout << "mx_x1y1: " << intrinsics->color_params.mx_x1y1 << std::endl; // xy // -0.000635062
        std::cout << "mx_x1y0: " << intrinsics->color_params.mx_x1y0 << std::endl; // x // 0.653884
        std::cout << "mx_x0y1: " << intrinsics->color_params.mx_x0y1 << std::endl; // y // 0.00151655
        std::cout << "mx_x0y0: " << intrinsics->color_params.mx_x0y0 << std::endl; // 1 // 0.126112
        std::cout << "my_x3y0: " << intrinsics->color_params.my_x3y0 << std::endl; // xxx // -3.59095e-06
        std::cout << "my_x0y3: " << intrinsics->color_params.my_x0y3 << std::endl; // yyy // 0.000861255
        std::cout << "my_x2y1: " << intrinsics->color_params.my_x2y1 << std::endl; // xxy // 0.000260318
        std::cout << "my_x1y2: " << intrinsics->color_params.my_x1y2 << std::endl; // yyx / 3.42709e-06
        std::cout << "my_x2y0: " << intrinsics->color_params.my_x2y0 << std::endl; // xx // -4.34207e-05
        std::cout << "my_x0y2: " << intrinsics->color_params.my_x0y2 << std::endl; // yy // -0.000753254
        std::cout << "my_x1y1: " << intrinsics->color_params.my_x1y1 << std::endl; // xy // 0.000458583
        std::cout << "my_x1y0: " << intrinsics->color_params.my_x1y0 << std::endl; // x // -0.00124378
        std::cout << "my_x0y1: " << intrinsics->color_params.my_x0y1 << std::endl; // y // 0.652787
        std::cout << "my_x0y0: " << intrinsics->color_params.my_x0y0 << std::endl; // 1 // -0.00915044

        std::cout << "-----ir_camera_params-----" << std::endl;
        std::cout << "fx: " << intrinsics->ir_params.fx << std::endl; // 364.885
        std::cout << "fy: " << intrinsics->ir_params.fy << std::endl; // 364.885
        std::cout << "cx: " << intrinsics->ir_params.cx << std::endl; // 254.166
        std::cout << "cy: " << intrinsics->ir_params.cy << std::endl; // 205.472
        std::cout << "k1: " << intrinsics->ir_params.k1 << std::endl; // 0.091112
        std::cout << "k2: " << intrinsics->ir_params.k2 << std::endl; // -0.27224
        std::cout << "k3: " << intrinsics->ir_params.k3 << std::endl; // 0.0960422
        std::cout << "p1: " << intrinsics->ir_params.p1 << std::endl; // 0
        std::cout << "p2: " << intrinsics->ir_params.p2 << std::endl; // 0
    }
    else
    {
        std::cout << "no kinect" << std::endl;
    }
}

void print_frame_info()
{
    auto device = kinect::obtainKinectDevice();
    if (!device)
    {
        std::cout << "no device" << std::endl;
        return;
    }

    std::string color_name = "kinect color frame";
    std::string depth_name = "kinect depth frame";
    cv::namedWindow(color_name, 1);
    cv::namedWindow(depth_name, 1);

    for (;;)
    {
        auto kinect_frame = device->getFrame();
        if (!kinect_frame)
        {
            std::cout << "no kinect_frame" << std::endl;
            Sleep(100);
            continue;
        }

        auto frame_interval = kinect_frame->color_frame()->getColorCameraSettings()->getFrameInterval();
        std::cout << "frame_interval: " << frame_interval << std::endl;

        auto yuv420_byte_frame = createDownsampledYuv420ByteFrameFromKinectColorFrame(kinect_frame->color_frame()->getRawUnderlyingBuffer());
        //std::cout << "width: " << yuv420_byte_frame.width() << std::endl;
        //std::cout << "y[0]: " << static_cast<int>(yuv420_byte_frame.y_channel()[0]) << std::endl;
        auto color_bgr_mat = convertYuv420ByteFrameToBgrMat(yuv420_byte_frame);

        auto rvl_frame = createRvlFrameFromKinectDepthFrame(kinect_frame->depth_frame()->getUnderlyingBuffer());
        std::cout << "rvl size: " << rvl_frame.size() << std::endl;
        auto depth_frame = createDepthFrameFromRvlFrame(rvl_frame.data());

        auto underlying_buffer = kinect_frame->depth_frame()->getUnderlyingBuffer();
        for (int i = 0; i < depth_frame.size(); ++i) {
            if (depth_frame[i] != underlying_buffer[i])
                std::cout << "a descrepency at " << i << std::endl;
        }

        std::cout << "print_frame_info - 1" << std::endl;
        auto depth_bgr_mat = convertKinectDepthFrameToBgrMat(depth_frame);
        std::cout << "print_frame_info - 2" << std::endl;

        cv::imshow(color_name, color_bgr_mat);
        std::cout << "print_frame_info - 3" << std::endl;
        cv::imshow(depth_name, depth_bgr_mat);
        if (cv::waitKey(1) >= 0)
            break;
    }
}
}

int main()
{
    rgbd_streamer::print_intrinsics();
    rgbd_streamer::print_frame_info();
    std::cin.get();
    return 0;
}