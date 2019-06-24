#pragma once

#include <iostream>
#include <asio.hpp>
#include <kinect/kinect.h>
#include "kh_message.h"

namespace kh
{
// Sends KinectIntrinsics and Kinect frames to a Receiver using the socket_.
// Receives socket_ through its constructor.
// Can receive messages from the Receiver that Kinect frames were successfully sent.
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