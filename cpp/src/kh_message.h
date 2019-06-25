#pragma once

#include <asio.hpp>
#include <optional>
#include <vector>

namespace kh
{
// A buffer implemented for our custom message protocol,
// which starts with an integer indicating the size of the actual message
// then includes the actual message.
class MessageBuffer
{
public:
    MessageBuffer();
    std::optional<std::vector<uint8_t>> receive(asio::ip::tcp::socket& socket);

private:
    // The buffer for the size of the actual mesasge.
    std::vector<uint8_t> size_bytes_;
    // Cursor of size_bytes_.
    size_t size_cursor_;
    // The buffer for the actual message.
    std::vector<uint8_t> message_bytes_;
    // Cursor of message_cursor_.
    size_t message_cursor_;
};

// A function for sending the buffer through the socket.
// Includes exception handling for network errors.
void sendMessageBuffer(asio::ip::tcp::socket& socket, std::vector<uint8_t>& buffer);
}