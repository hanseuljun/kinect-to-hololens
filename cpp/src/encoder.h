#pragma once

#include <exception>
#include <memory>
#include <vpx/vp8cx.h>
#include <vpx/vpx_codec.h>
#include "frames.h"

namespace rgbd_streamer
{
class ColorEncoder
{
public:
    ColorEncoder(vpx_codec_ctx_t codec, vpx_image_t image);
    ~ColorEncoder();
    std::vector<uint8_t> encode(YuvFrame& av_frame);

private:
    vpx_codec_ctx_t codec_;
    vpx_image_t image_;
    int keyframe_interval_;
    int frame_index_;
};

std::unique_ptr<ColorEncoder> createColorEncoder(int width, int height, int target_bitrate);
}