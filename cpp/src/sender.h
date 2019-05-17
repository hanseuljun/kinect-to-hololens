#pragma once

#include <iostream>
#include <asio.hpp>

namespace kh
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

    void send(int frame_id, std::vector<uint8_t>& encoder_frame, std::vector<uint8_t> rvl_frame)
    {
        uint32_t packet_size = 1 + 4 + 4 + encoder_frame.size() + 4 + rvl_frame.size();
        uint32_t buffer_size = 4 + packet_size;

        std::vector<uint8_t> buffer(buffer_size);
        size_t cursor = 0;

        memcpy(buffer.data() + cursor, &packet_size, 4);
        cursor += 4;

        buffer[4] = static_cast<uint8_t>(1);
        cursor += 1;
        
        memcpy(buffer.data() + cursor, &frame_id, 4);
        cursor += 4;

        auto encoder_frame_size = static_cast<uint32_t>(encoder_frame.size());
        memcpy(buffer.data() + cursor, &encoder_frame_size, 4);
        cursor += 4;
        
        memcpy(buffer.data() + cursor, encoder_frame.data(), encoder_frame.size());
        cursor += encoder_frame.size();

        auto rvl_frame_size = static_cast<uint32_t>(rvl_frame.size());
        memcpy(buffer.data() + cursor, &rvl_frame_size, 4);
        cursor += 4;

        memcpy(buffer.data() + cursor, rvl_frame.data(), rvl_frame.size());
        cursor += rvl_frame.size();

        std::error_code error_code;

        // Bytes may not be sent. It usually happens due to network congestion.
        // The current solution is to try again until the bytes get sent.
        for (;;) {
            int size = socket_.send(asio::buffer(buffer.data(), buffer_size), 0, error_code);
            if (size == buffer_size)
                break;
            if (error_code && (error_code != asio::error::would_block))
                throw std::exception("Server error");
            if (size != 0)
                throw std::exception("Sender partially sent the buffer...");
        }
    }

private:
    asio::ip::tcp::acceptor acceptor_;
    asio::ip::tcp::socket socket_;
    bool accepted_;
};
}