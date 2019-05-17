#pragma once

#include <iostream>
#include <asio.hpp>
#include <kinect/kinect.h>

namespace kh
{
class Sender
{
public:
    Sender(asio::ip::tcp::socket&& socket);
    void send(kinect::KinectIntrinsics intrinsics);
    void send(int frame_id, std::vector<uint8_t>& vp8_frame, std::vector<uint8_t> rvl_frame);
private:
    void _send(std::vector<uint8_t>& buffer);

private:
    asio::ip::tcp::socket socket_;
    bool accepted_;
};
}