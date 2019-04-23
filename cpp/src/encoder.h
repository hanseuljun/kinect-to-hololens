#pragma once

#include <exception>
#include <memory>
#include <vpx/vp8cx.h>
#include <vpx/vpx_codec.h>
#include "frames.h"

namespace rgbd_streamer
{
template <typename T>
class LibvpxEncoder
{
public:
    LibvpxEncoder(vpx_codec_ctx_t codec, vpx_image_t image);
    ~LibvpxEncoder();
    std::vector<uint8_t> encode(Yuv420Frame<T>& frame);
    static std::vector<uint8_t> encodeFrame(vpx_codec_ctx_t* codec,
        vpx_image_t* image,
        int frame_index,
        int flags);

private:
    vpx_codec_ctx_t codec_;
    vpx_image_t image_;
    int keyframe_interval_;
    int frame_index_;
};

std::unique_ptr<LibvpxEncoder<uint8_t>> createVp8Encoder(int width, int height, int target_bitrate);
}