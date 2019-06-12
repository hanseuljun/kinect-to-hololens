#include "kh_message.h"

namespace kh
{
MessageBuffer::MessageBuffer()
    : size_bytes_(4)
    , size_cursor_(0)
    , message_bytes_()
    , message_cursor_(0)
{
}

// Returns a message if there is one available.
// Returns an std::nullopt if no message is found.
// Throw an excpetion if there is an error.
std::optional<std::vector<uint8_t>> MessageBuffer::receive(asio::ip::tcp::socket& socket)
{
    if (size_cursor_ < size_bytes_.size()) {
        std::error_code error_code;
        int size_result = socket.receive(asio::buffer(size_bytes_.data() + size_cursor_, size_bytes_.size() - size_cursor_), 0, error_code);
        if (error_code && error_code != asio::error::would_block) {
            auto error_message = std::string("Error receiving message size: ") + std::to_string(error_code.value());
            throw std::exception(error_message.c_str());
        } else {
            size_cursor_ += size_result;
        }

        if (size_cursor_ == size_bytes_.size()) {
            int message_size;
            memcpy(&message_size, size_bytes_.data(), 4);
            message_bytes_ = std::vector<uint8_t>(message_size);
        } else {
            return std::nullopt;
        }
    }

    std::error_code error_code;
    int message_result = socket.receive(asio::buffer(message_bytes_.data() + message_cursor_, message_bytes_.size() - message_cursor_), 0, error_code);
    if (error_code && error_code != asio::error::would_block) {
        auto error_message = std::string("Error receiving message: ") + std::to_string(error_code.value());
        throw std::exception(error_message.c_str());
    } else {
        message_cursor_ += message_result;
    }

    if (message_cursor_ < message_bytes_.size())
        return std::nullopt;

    size_cursor_ = 0;
    message_cursor_ = 0;

    return message_bytes_;
}

void sendMessageBuffer(asio::ip::tcp::socket& socket, std::vector<uint8_t>& buffer)
{
    std::error_code error_code;

    // Bytes may not be sent. It usually happens due to network congestion.
    // The current solution is to try again until the bytes get sent.
    for(;;) {
        size_t size = socket.send(asio::buffer(buffer.data(), buffer.size()), 0, error_code);
        if(size == buffer.size())
            break;
        if(error_code && (error_code != asio::error::would_block))
            throw std::exception("An error from a sender...");
        if(size != 0)
            throw std::exception("A sender sent the buffer partially...");
    }
}
}