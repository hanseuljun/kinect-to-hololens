#include "kh_receiver.h"

namespace kh
{
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

void Receiver::send(int frame_id)
{
    uint32_t message_size = static_cast<uint32_t>(1 + 4);
    uint32_t buffer_size = static_cast<uint32_t>(4 + message_size);

    std::vector<uint8_t> buffer(buffer_size);
    size_t cursor = 0;

    memcpy(buffer.data() + cursor, &message_size, 4);
    cursor += 4;

    // Message type
    buffer[4] = static_cast<uint8_t>(0);
    cursor += 1;

    memcpy(buffer.data() + cursor, &frame_id, 4);

    sendMessageBuffer(socket_, buffer);
}
}