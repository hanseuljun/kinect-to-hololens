#include <iostream>
#include <asio.hpp>

#include "kinect/kinect.h"
#include "sender.h"
#include "frames.h"
#include "vp8.h"

namespace kh
{
void send_frames()
{
    const int PORT = 7777;

    auto device = kinect::obtainKinectDevice();
    if (!device) {
        std::cout << "Could not find a kinect." << std::endl;
        return;
    }


    auto intrinsics = kinect::obtainKinectIntrinsics();

    Vp8Encoder encoder(960, 540, 2000);

    asio::io_context io_context;
    Sender sender(io_context, PORT);
    sender.accept();

    bool stopped = false;
    std::thread thread([&io_context, &stopped]() {
        while (!stopped)
            io_context.run();
    });

    int frame_id = 0;
    for (;;) {
        if (!sender.accepted())
            continue;

        auto kinect_frame = device->acquireFrame();
        if (!kinect_frame)
            continue;

        auto yuv_frame = createHalfSizedYuvImageFromKinectColorBuffer(kinect_frame->color_frame()->getRawUnderlyingBuffer());
        auto encoder_frame = encoder.encode(yuv_frame);

        auto rvl_frame = createRvlFrameFromKinectDepthBuffer(kinect_frame->depth_frame()->getUnderlyingBuffer());

        try {
            sender.send(frame_id++, encoder_frame, rvl_frame);
        } catch (std::exception& e) {
            std::cout << e.what() << std::endl;
            break;
        }
    }

    stopped = true;
    io_context.stop();
    thread.join();
}
}

int main()
{
    kh::send_frames();

    return 0;
}