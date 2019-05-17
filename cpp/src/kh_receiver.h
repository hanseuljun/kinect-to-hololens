#pragma once

#include <iostream>
#include <asio.hpp>
#include <optional>
#include <vector>

namespace kh
{
class MessageBuffer
{
public:
    MessageBuffer();
    std::optional<std::vector<uint8_t>> receive(asio::ip::tcp::socket& socket);

private:
    std::vector<uint8_t> size_bytes_;
    int size_cursor_;
    std::vector<uint8_t> message_bytes_;
    int message_cursor_;
};

class Receiver
{
public:
    Receiver(asio::io_context& io_context);
    bool connect(std::string ip_address, int port);
    std::optional<std::vector<uint8_t>> receive();
    
private:
    asio::ip::tcp::socket socket_;
    MessageBuffer message_buffer_;
};
}