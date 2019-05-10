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
    Receiver(asio::io_context& io_context)
        : socket_(io_context), message_buffer_()
    {
    }

    bool connect(std::string ip_address, int port)
    {
        asio::ip::tcp::resolver resolver(socket_.get_io_context());
        auto endpoints = resolver.resolve(ip_address, std::to_string(port));
        std::error_code error;
        asio::connect(socket_, endpoints, error);

        if (error && error != asio::error::would_block)
            return false;

        return socket_.is_open();
    }

    std::optional<std::vector<uint8_t>> receive()
    {
        return message_buffer_.receive(socket_);
    }

private:
    asio::ip::tcp::socket socket_;
    MessageBuffer message_buffer_;
};
}