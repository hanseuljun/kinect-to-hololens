#include <iostream>
#include <asio.hpp>
#include "receiver.h"
#include "decoder.h"
#include "helper/opencv_helper.h"

namespace kh
{
void receive_frames()
{
    const std::string IP_ADDRESS = "127.0.0.1";
    const int PORT = 7777;

    asio::io_context io_context;
    Receiver receiver(io_context);
    receiver.connect(IP_ADDRESS, PORT);

    auto color_name = "kinect color frame";
    auto depth_name = "kinect depth frame";

    Vp8Decoder decoder;

    for (;;) {
        auto receive_result = receiver.receive();
        if (receive_result) {
            int cursor = 0;
            cursor += 1;

            int frame_id;
            memcpy(&frame_id, receive_result->data() + cursor, 4);
            std::cout << "frame_id: " << frame_id << std::endl;
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

            auto decoder_frame = decoder.decode(encoder_frame);
            auto color_mat = createCvMatFromYuvImage(createYuvImageFromAvFrame(decoder_frame.av_frame()));

            auto depth_frame = createDepthFrameFromRvlFrame(rvl_frame.data());
            auto depth_mat = createCvMatFromKinectDepthBuffer(depth_frame.data());

            cv::imshow(color_name, color_mat);
            cv::imshow(depth_name, depth_mat);
            if (cv::waitKey(1) >= 0)
                break;
        }
    }
}
}

int main()
{
    kh::receive_frames();

    return 0;
}