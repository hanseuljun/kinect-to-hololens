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

    asio::io_context io_context;
    Receiver receiver(io_context);
    for (;;) {
        if (receiver.connect(ip_address, port))
            break;
    }

    std::cout << "Connected!" << std::endl;

    Vp8Decoder decoder;
    for (;;) {
        auto receive_result = receiver.receive();
        if (receive_result) {
            int cursor = 0;
            cursor += 1;

            int frame_id;
            memcpy(&frame_id, receive_result->data() + cursor, 4);
            cursor += 4;

            int encoder_frame_size;
            memcpy(&encoder_frame_size, receive_result->data() + cursor, 4);
            cursor += 4;

            std::vector<uint8_t> encoder_frame(encoder_frame_size);
            memcpy(encoder_frame.data(), receive_result->data() + cursor, encoder_frame_size);
            cursor += encoder_frame_size;

            int rvl_frame_size;
            memcpy(&rvl_frame_size, receive_result->data() + cursor, 4);
            cursor += 4;

            std::vector<uint8_t> rvl_frame(rvl_frame_size);
            memcpy(rvl_frame.data(), receive_result->data() + cursor, rvl_frame_size);
            cursor += rvl_frame_size;

            if (frame_id % 100 == 0)
                std::cout << "Received frame " << frame_id << "." << std::endl;

            auto decoder_frame = decoder.decode(encoder_frame);
            auto color_mat = createCvMatFromYuvImage(createYuvImageFromAvFrame(decoder_frame.av_frame()));

            auto depth_frame = createDepthFrameFromRvlFrame(rvl_frame.data());
            auto depth_mat = createCvMatFromKinectDepthBuffer(depth_frame.data());

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
        std::cout << "Enter an IP address to start sending frames: ";
        std::string ip_address;
        std::getline(std::cin, ip_address);
        if (ip_address.empty())
            ip_address = "127.0.0.1";

        std::cout << "Enter a port number to start sending frames: ";
        std::string port_line;
        std::getline(std::cin, port_line);
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