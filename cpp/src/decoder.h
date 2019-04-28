#pragma once

#include <memory>
#include <vector>

#include "frames.h"

namespace rgbd_streamer
{
class ColorDecoder
{
public:
    ColorDecoder(AVCodecID av_codec_id);
    ~ColorDecoder();
    ColorDecoderFrame decode(const std::vector<uint8_t>& av_frame);

private:
    AVPacket* packet_;
    AVCodec* codec_;
    AVCodecParserContext* codec_parser_context_;
    AVCodecContext* codec_context_;
};

std::unique_ptr<ColorDecoder> createColorDecoder();
}