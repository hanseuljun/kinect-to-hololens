#pragma once

#include <iostream>
#include <asio.hpp>

namespace kh
{
class Sender
{
public:
    Sender(asio::ip::tcp::socket&& socket);
    void send(int frame_id, std::vector<uint8_t>& encoder_frame, std::vector<uint8_t> rvl_frame);

private:
    asio::ip::tcp::socket socket_;
    bool accepted_;
};
}