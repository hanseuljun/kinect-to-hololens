#pragma once

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

void sendMessageBuffer(asio::ip::tcp::socket& socket, std::vector<uint8_t>& buffer);
}