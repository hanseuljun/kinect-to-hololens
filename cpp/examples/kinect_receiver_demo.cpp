#include <iostream>
#include <asio.hpp>
#include "kh_receiver.h"
#include "kh_vp8.h"
#include "helper/opencv_helper.h"

namespace kh
{
void _receive_frames(std::string ip_address, int port)
{
    std::cout << "Try connecting to " << ip_address << ":" << port << std::endl;

    // Try connecting to a Sender with the IP address and the port.
    asio::io_context io_context;
    Receiver receiver(io_context);
    for (;;) {
        if (receiver.connect(ip_address, port))
            break;
    }

    std::cout << "Connected!" << std::endl;

    Vp8Decoder decoder;
    for (;;) {
        // Try receiving a message from the Receiver.
        auto receive_result = receiver.receive();
        // Keep trying again if there is none.
        if(!receive_result)
            continue;

        int cursor = 0;

        auto message_type = (*receive_result)[0];
        cursor += 1;

        // There can be two types of messages: a KinectIntrinsics and a Kinect frame.
        if (message_type == 0) {
            // This application has nothing to do with a KinectIntrinsics that is for 3D rendering
            // since it uses OpenCV to render the frames in 2D.
            std::cout << "Received intrinsics." << std::endl;
        } else if (message_type == 1) {
            // Parse the ID of the frame and send a feedback meesage to the sender
            // to indicate the frame was succesfully received.
            // This is required to minimize the end-to-end latency from the Kinect of the Sender
            // and the display of the Receiver.
            int frame_id;
            memcpy(&frame_id, receive_result->data() + cursor, 4);
            cursor += 4;

            if(frame_id % 100 == 0)
                std::cout << "Received frame " << frame_id << "." << std::endl;
            receiver.send(frame_id);

            // Parsing the bytes of the message into the VP8 and RVL frames.
            int vp8_frame_size;
            memcpy(&vp8_frame_size, receive_result->data() + cursor, 4);
            cursor += 4;

            std::vector<uint8_t> vp8_frame(vp8_frame_size);
            memcpy(vp8_frame.data(), receive_result->data() + cursor, vp8_frame_size);
            cursor += vp8_frame_size;

            int rvl_frame_size;
            memcpy(&rvl_frame_size, receive_result->data() + cursor, 4);
            cursor += 4;

            std::vector<uint8_t> rvl_frame(rvl_frame_size);
            memcpy(rvl_frame.data(), receive_result->data() + cursor, rvl_frame_size);
            cursor += rvl_frame_size;

            // Decoding a Vp8Frame into color pixels.
            auto ffmpeg_frame = decoder.decode(vp8_frame.data(), vp8_frame.size());
            auto color_mat = createCvMatFromYuvImage(createYuvImageFromAvFrame(ffmpeg_frame.av_frame()));

            // Decompressing a RVL frame into depth pixels.
            auto depth_image = createDepthImageFromRvlFrame(rvl_frame.data());
            auto depth_mat = createCvMatFromKinectDepthImage(depth_image.data());

            // Rendering the depth pixels.
            cv::imshow("Color", color_mat);
            cv::imshow("Depth", depth_mat);
            if (cv::waitKey(1) >= 0)
                break;
        }
    }
}

void receive_frames()
{
    for (;;) {
        // Receive IP address from the user.
        std::cout << "Enter an IP address to start sending frames: ";
        std::string ip_address;
        std::getline(std::cin, ip_address);
        // The default IP address is 127.0.0.1.
        if (ip_address.empty())
            ip_address = "127.0.0.1";

        // Receive port from the user.
        std::cout << "Enter a port number to start sending frames: ";
        std::string port_line;
        std::getline(std::cin, port_line);
        // The default port is 7777.
        int port = port_line.empty() ? 7777 : std::stoi(port_line);
        try {
            _receive_frames(ip_address, port);
        } catch (std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }
}
}

int main()
{
    kh::receive_frames();
    return 0;
}