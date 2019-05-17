#pragma once

#include "frames.h"

namespace kh
{
class Vp8Decoder
{
public:
    Vp8Decoder();
    ~Vp8Decoder();
    ColorDecoderFrame decode(const std::vector<uint8_t>& av_frame);

private:
    AVPacket* packet_;
    AVCodec* codec_;
    AVCodecParserContext* codec_parser_context_;
    AVCodecContext* codec_context_;
};
}