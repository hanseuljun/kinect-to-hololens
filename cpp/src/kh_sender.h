#pragma once

#include <iostream>
#include <asio.hpp>
#include <kinect/kinect.h>
#include "kh_message.h"

namespace kh
{
class Sender
{
public:
    Sender(asio::ip::tcp::socket&& socket);
    void send(kinect::KinectIntrinsics intrinsics);
    void send(int frame_id, std::vector<uint8_t>& vp8_frame, std::vector<uint8_t> rvl_frame);
    std::optional<std::vector<uint8_t>> receive();

private:
    asio::ip::tcp::socket socket_;
    MessageBuffer message_buffer_;
};
}