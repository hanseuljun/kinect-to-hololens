#include "kh_receiver.h"

namespace kh
{
MessageBuffer::MessageBuffer()
    : size_bytes_(4)
    , size_cursor_(0)
    , message_bytes_()
    , message_cursor_(0)
{
}

// Returns a Packet if a packet is found.
// Returns an std::nullopt if no packet is found.
// Throw an excpetion if there is an error.
std::optional<std::vector<uint8_t>> MessageBuffer::receive(asio::ip::tcp::socket& socket)
{
    if (size_cursor_ < size_bytes_.size()) {
        std::error_code error_code;
        int packet_size_result = socket.receive(asio::buffer(size_bytes_.data() + size_cursor_, size_bytes_.size() - size_cursor_), 0, error_code);
        if (error_code && error_code != asio::error::would_block) {
            auto error_message = std::string("Error receiving packet size: ") + std::to_string(error_code.value());
            throw std::exception(error_message.c_str());
        } else {
            size_cursor_ += packet_size_result;
        }

        if (size_cursor_ == size_bytes_.size()) {
            int packet_size;
            memcpy(&packet_size, size_bytes_.data(), 4);
            message_bytes_ = std::vector<uint8_t>(packet_size);
        } else {
            return std::nullopt;
        }
    }

    std::error_code error_code;
    int packet_result = socket.receive(asio::buffer(message_bytes_.data() + message_cursor_, message_bytes_.size() - message_cursor_), 0, error_code);
    if (error_code && error_code != asio::error::would_block) {
        auto error_message = std::string("Error receiving packet: ") + std::to_string(error_code.value());
        throw std::exception(error_message.c_str());
    } else {
        message_cursor_ += packet_result;
    }

    if (message_cursor_ < message_bytes_.size())
        return std::nullopt;

    size_cursor_ = 0;
    message_cursor_ = 0;

    return message_bytes_;
}

Receiver::Receiver(asio::io_context& io_context)
    : socket_(io_context), message_buffer_()
{
}

bool Receiver::connect(std::string ip_address, int port)
{
    asio::ip::tcp::resolver resolver(socket_.get_io_context());
    auto endpoints = resolver.resolve(ip_address, std::to_string(port));
    std::error_code error;
    asio::connect(socket_, endpoints, error);

    if (error && error != asio::error::would_block)
        return false;

    return socket_.is_open();
}

std::optional<std::vector<uint8_t>> Receiver::receive()
{
    return message_buffer_.receive(socket_);
}
}