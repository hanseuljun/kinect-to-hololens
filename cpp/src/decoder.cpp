#include "decoder.h"

#include <iostream>
#include <libavformat/avformat.h>

namespace rgbd_streamer
{
ColorDecoder::ColorDecoder(AVPacket* packet, AVCodec* codec,
    AVCodecParserContext* codec_parser_context,
    AVCodecContext* codec_context)
    : packet_(packet)
    , codec_(codec)
    , codec_parser_context_(codec_parser_context)
    , codec_context_(codec_context)
{
}

ColorDecoder::~ColorDecoder()
{
    if (packet_)
        av_packet_free(&packet_);

    if (codec_parser_context_)
        av_parser_close(codec_parser_context_);

    if (codec_context_)
        avcodec_free_context(&codec_context_);
}

void decodePacket(std::vector<ColorDecoderFrame>& decoder_frames, AVCodecContext* codec_context, AVPacket* pkt)
{
    if (avcodec_send_packet(codec_context, pkt) < 0)
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

ColorDecoderFrame ColorDecoder::decode(const std::vector<uint8_t>& av_frame)
{
    std::vector<ColorDecoderFrame> decoder_frames;
    /* use the parser to split the data into frames */
    size_t data_size = av_frame.size();
    // Adding buffer padding is important!
    // Removing this results in crashes that happens in a way hard to debug!!! (I know it since it happened to me...)
    // When it happens, it happens with av_parser_parse2.
    std::unique_ptr<uint8_t> padded_data(new uint8_t[data_size + AV_INPUT_BUFFER_PADDING_SIZE]);
    memcpy(padded_data.get(), av_frame.data(), data_size);
    memset(padded_data.get() + data_size, 0, AV_INPUT_BUFFER_PADDING_SIZE);
    uint8_t* data = padded_data.get();

    while (data_size > 0) {
        // Returns the number of bytes used.
        int size = av_parser_parse2(codec_parser_context_,
            codec_context_, &packet_->data, &packet_->size,
            data, data_size, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);

        if (size < 0)
            throw std::exception("Error from av_parser_parse2.");
        
        data += size;
        data_size -= size;

        if (packet_->size)
            decodePacket(decoder_frames, codec_context_, packet_);
    }

    if (decoder_frames.size() != 1) {
        throw std::exception("More or less than a frame found in ColorDecoder::decode.");
    }

    return std::move(decoder_frames[0]);
}

std::unique_ptr<ColorDecoder> createColorDecoder()
{
    av_log_set_level(AV_LOG_INFO);
    
    const AVCodecID AV_CODEC_ID = AV_CODEC_ID_VP8;

    auto packet = av_packet_alloc();
    if (!packet)
        throw std::exception("av_packet_alloc failed.");

    avcodec_register_all();
    auto codec = avcodec_find_decoder(AV_CODEC_ID);
    if (!codec)
        throw std::exception("avcodec_find_decoder failed.");

    auto codec_parser_context = av_parser_init(codec->id);
    if (!codec_parser_context)
        throw std::exception("av_parser_init failed.");

    auto codec_context = avcodec_alloc_context3(codec);
    if (!codec_context)
        throw std::exception("avcodec_alloc_context3 failed.");

    /* open it */
    if (avcodec_open2(codec_context, codec, nullptr) < 0)
        throw std::exception("avcodec_open2 failed.");
    
    return std::make_unique<ColorDecoder>(packet, codec, codec_parser_context, codec_context);
}
}