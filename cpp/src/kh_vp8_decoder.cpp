#include "kh_vp8.h"

#include <iostream>
#include <libavformat/avformat.h>

namespace kh
{
Vp8Decoder::Vp8Decoder()
    : packet_(nullptr)
    , codec_(nullptr)
    , codec_parser_context_(nullptr)
    , codec_context_(nullptr)
{
    packet_ = av_packet_alloc();
    if (!packet_)
        throw std::exception("av_packet_alloc failed.");

    codec_ = avcodec_find_decoder(AV_CODEC_ID_VP8);
    if (!codec_)
        throw std::exception("avcodec_find_decoder failed.");

    codec_parser_context_ = av_parser_init(codec_->id);
    if (!codec_parser_context_)
        throw std::exception("av_parser_init failed.");

    codec_context_ = avcodec_alloc_context3(codec_);
    if (!codec_context_)
        throw std::exception("avcodec_alloc_context3 failed.");

    /* open it */
    if (avcodec_open2(codec_context_, codec_, nullptr) < 0)
        throw std::exception("avcodec_open2 failed.");
}

Vp8Decoder::~Vp8Decoder()
{
    if (packet_)
        av_packet_free(&packet_);

    if (codec_parser_context_)
        av_parser_close(codec_parser_context_);

    if (codec_context_)
        avcodec_free_context(&codec_context_);
}

// A helper function for Vp8Decoder::decode() that feeds frames of packet into decoder_frames.
void decodePacket(std::vector<FFmpegFrame>& decoder_frames, AVCodecContext* codec_context, AVPacket* packet)
{
    if (avcodec_send_packet(codec_context, packet) < 0)
        throw std::exception("Error from avcodec_send_packet.");

    while (true) {
        auto av_frame = av_frame_alloc();
        if (!av_frame)
            throw std::exception("Error from av_frame_alloc.");
        
        av_frame->format = AV_PIX_FMT_YUV420P;

        int receive_frame_result = avcodec_receive_frame(codec_context, av_frame);
        if (receive_frame_result == AVERROR(EAGAIN) || receive_frame_result == AVERROR_EOF) {
            return;
        } else if (receive_frame_result < 0) {
            throw std::exception("Error from avcodec_send_packet.");
        }

        fflush(stdout);
        decoder_frames.emplace_back(av_frame);
    }

    return;
}

// Decode frames in vp8_frame_data.
FFmpegFrame Vp8Decoder::decode(uint8_t* vp8_frame_data, size_t vp8_frame_size)
{
    std::vector<FFmpegFrame> decoder_frames;
    /* use the parser to split the data into frames */
    size_t data_size = vp8_frame_size;
    // Adding buffer padding is important!
    // Removing this will result in crashes in some cases.
    // When the crash happens, it happens in av_parser_parse2().
    std::unique_ptr<uint8_t> padded_data(new uint8_t[data_size + AV_INPUT_BUFFER_PADDING_SIZE]);
    memcpy(padded_data.get(), vp8_frame_data, data_size);
    memset(padded_data.get() + data_size, 0, AV_INPUT_BUFFER_PADDING_SIZE);
    uint8_t* data = padded_data.get();

    while (data_size > 0) {
        // Returns the number of bytes used.
        int size = av_parser_parse2(codec_parser_context_,
            codec_context_, &packet_->data, &packet_->size,
            data, static_cast<int>(data_size), AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);

        if (size < 0)
            throw std::exception("An error from av_parser_parse2.");
        
        data += size;
        data_size -= size;

        if (packet_->size)
            decodePacket(decoder_frames, codec_context_, packet_);
    }

    if (decoder_frames.size() != 1)
        throw std::exception("More or less than one frame found in Vp8Decoder::decode.");

    return std::move(decoder_frames[0]);
}
}