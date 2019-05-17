#include "kh_sender.h"

namespace kh
{
Sender::Sender(asio::ip::tcp::socket&& socket)
    : socket_(std::move(socket))
{
    socket_.non_blocking(true);
}

void Sender::send(kinect::KinectIntrinsics intrinsics)
{
    static_assert(sizeof(intrinsics.color_params) == 26 * 4);
    static_assert(sizeof(intrinsics.ir_params) == 9 * 4);

    uint32_t message_size = static_cast<uint32_t>(1 + sizeof(intrinsics.color_params) + sizeof(intrinsics.ir_params));
    uint32_t buffer_size = static_cast<uint32_t>(4 + message_size);

    std::vector<uint8_t> buffer(buffer_size);
    size_t cursor = 0;

    memcpy(buffer.data() + cursor, &message_size, 4);
    cursor += 4;

    // Message type
    buffer[4] = static_cast<uint8_t>(0);
    cursor += 1;

    memcpy(buffer.data() + cursor, &intrinsics.color_params, sizeof(intrinsics.color_params));
    cursor += 4;

    memcpy(buffer.data() + cursor, &intrinsics.ir_params, sizeof(intrinsics.ir_params));
    cursor += 4;

    _send(buffer);
}

void Sender::send(int frame_id, std::vector<uint8_t>& encoder_frame, std::vector<uint8_t> rvl_frame)
{
    uint32_t message_size = static_cast<uint32_t>(1 + 4 + 4 + encoder_frame.size() + 4 + rvl_frame.size());
    uint32_t buffer_size = static_cast<uint32_t>(4 + message_size);

    std::vector<uint8_t> buffer(buffer_size);
    size_t cursor = 0;

    memcpy(buffer.data() + cursor, &message_size, 4);
    cursor += 4;

    // Message type
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

    _send(buffer);
}

void Sender::_send(std::vector<uint8_t>& buffer)
{
    std::error_code error_code;

    // Bytes may not be sent. It usually happens due to network congestion.
    // The current solution is to try again until the bytes get sent.
    for (;;) {
        size_t size = socket_.send(asio::buffer(buffer.data(), buffer.size()), 0, error_code);
        if (size == buffer.size())
            break;
        if (error_code && (error_code != asio::error::would_block))
            throw std::exception("An error from a sender...");
        if (size != 0)
            throw std::exception("A sender sent the buffer partially...");
    }
}
}