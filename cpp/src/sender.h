#pragma once

#include <iostream>
#include <asio.hpp>

namespace rgbd_streamer
{
class Sender
{
public:
    Sender(asio::io_context& io_context, int port)
        : acceptor_(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
        , socket_(io_context)
        , accepted_(false)
    {
        acceptor_.non_blocking(true);
    }

    void accept()
    {
        acceptor_.async_accept(socket_, [&](std::error_code ec) {
            if (!ec) {
                socket_.non_blocking(true);
                accepted_ = true;
            }
        });
    }

    bool accepted()
    {
        return accepted_;
    }

    int send(int n)
    {
        int buffer_size = 8;
        std::vector<uint8_t> buffer(buffer_size);
        int int_size = 4;
        memcpy(buffer.data(), &int_size, 4);
        memcpy(buffer.data() + 4, &n, 4);

        std::error_code error_code;
        int size = socket_.send(asio::buffer(buffer.data(), buffer_size), 0, error_code);

        if (error_code && (error_code != asio::error::would_block))
            throw std::exception("server error");

        return size;
    }

private:
    asio::ip::tcp::acceptor acceptor_;
    asio::ip::tcp::socket socket_;
    bool accepted_;
};
}