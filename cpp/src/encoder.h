#pragma once

#include <vpx/vp8cx.h>
#include <vpx/vpx_codec.h>
#include "frames.h"

namespace kh
{
class Vp8Encoder
{
public:
    Vp8Encoder(int width, int height, int target_bitrate);
    ~Vp8Encoder();
    std::vector<uint8_t> encode(YuvImage& yuv_image);

private:
    vpx_codec_ctx_t codec_;
    vpx_image_t image_;
    int keyframe_interval_;
    int frame_index_;
};
}