#include "kh_core.h"
#include "kh_sender.h"
#include "kh_vp8.h"
#include "kinect/kinect.h"

namespace kh
{
void _send_frames(int port)
{
    std::cout << "Start sending Kinect frames (port: " << port << ")." << std::endl;

    auto intrinsics = kinect::obtainKinectIntrinsics();

    auto device = kinect::obtainKinectDevice();
    if (!device) {
        std::cout << "Could not find a kinect device." << std::endl;
        return;
    }

    Vp8Encoder encoder(960, 540, 2000);

    asio::io_context io_context;
    asio::ip::tcp::acceptor acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port));
    auto socket = acceptor.accept();
    
    std::cout << "Accepted a client!" << std::endl;

    Sender sender(std::move(socket));

    bool stopped = false;
    std::thread thread([&io_context, &stopped]() {
        while (!stopped)
            io_context.run();
    });

    int frame_id = 0;
    for (;;) {
        auto kinect_frame = device->acquireFrame();
        if (!kinect_frame)
            continue;

        auto yuv_image = createHalfSizedYuvImageFromKinectColorBuffer(kinect_frame->color_frame()->getRawUnderlyingBuffer());
        auto encoder_frame = encoder.encode(yuv_image);

        auto rvl_frame = createRvlFrameFromKinectDepthBuffer(kinect_frame->depth_frame()->getUnderlyingBuffer());

        if (frame_id % 100 == 0)
            std::cout << "Sending frame " << frame_id << "." << std::endl;

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

    std::cout << "Stopped sending Kinect frames." << std::endl;
}

void send_frames()
{
    for (;;) {
        std::cout << "Enter a port number to start sending frames: ";
        std::string line;
        std::getline(std::cin, line);
        int port = line.empty() ? 7777 : std::stoi(line);
        try {
            _send_frames(port);
        } catch (std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }
}
}

int main()
{
    kh::send_frames();
    return 0;
}