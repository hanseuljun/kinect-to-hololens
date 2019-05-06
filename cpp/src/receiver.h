#pragma once

#include <iostream>
#include <asio.hpp>
#include <optional>
#include <vector>

namespace rgbd_streamer
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

    void connect(std::string ip_address, int port)
    {
        asio::ip::tcp::resolver resolver(socket_.get_io_context());
        asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(ip_address, std::to_string(port));
        std::error_code connect_error;
        asio::connect(socket_, endpoints, connect_error);

        if (connect_error && connect_error != asio::error::would_block) {
            std::cout << "not connected" << std::endl;
            return;
        }

        if (socket_.is_open()) {
            std::cout << "connected" << std::endl;
        } else {
            std::cout << "not connected" << std::endl;
        }
    }

    void receive()
    {
        auto receive_result = message_buffer_.receive(socket_);
        std::cout << "receive: " << receive_result.has_value() << std::endl;

        if (receive_result) {
            int n;
            memcpy(&n, receive_result.value().data(), 4);
            std::cout << "n: " << n << std::endl;
        }
    }

private:
    asio::ip::tcp::socket socket_;
    MessageBuffer message_buffer_;
};
}