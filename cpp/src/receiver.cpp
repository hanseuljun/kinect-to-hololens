#include "receiver.h"

namespace rgbd_streamer
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
            std::cout << "error_code: " << error_code << std::endl;
            throw std::exception("error in MessageBuffer::receivePacket1");
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
        std::cout << "error_code: " << error_code << std::endl;
        throw std::exception("error in MessageBuffer::receivePacket2");
    } else {
        message_cursor_ += packet_result;
    }

    if (message_cursor_ < message_bytes_.size())
        return std::nullopt;

    size_cursor_ = 0;
    message_cursor_ = 0;

    return message_bytes_;
}
}