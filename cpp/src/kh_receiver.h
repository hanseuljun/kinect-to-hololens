#pragma once

#include <iostream>
#include <asio.hpp>
#include <optional>
#include "kh_message.h"

namespace kh
{
// Receives messages from a remote Kinect using a socket created by
// an asio::io_context of its constructor.
// Can connect to a Sender given an IP address and a port,
// and can send feedbacks to the Sender when it receives Kinect frames.
class Receiver
{
public:
    Receiver(asio::io_context& io_context);
    bool connect(std::string ip_address, int port);
    std::optional<std::vector<uint8_t>> receive();
    void send(int frame_id);
    
private:
    asio::ip::tcp::socket socket_;
    MessageBuffer message_buffer_;
};
}