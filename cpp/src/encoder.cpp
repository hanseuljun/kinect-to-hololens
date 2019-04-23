#include "encoder.h"

#include <iostream>

namespace rgbd_streamer
{
template class LibvpxEncoder<uint8_t>;
template class LibvpxEncoder<uint16_t>;

template <typename T>
LibvpxEncoder<T>::LibvpxEncoder(vpx_codec_ctx_t codec, vpx_image_t image)
    : codec_(codec), image_(image), keyframe_interval_(30), frame_index_(0)
{
}

template <typename T>
LibvpxEncoder<T>::~LibvpxEncoder()
{
    vpx_img_free(&image_);
    if (vpx_codec_destroy(&codec_))
        std::cout << "Failed to destroy codec in LibvpxEncoder::~LibvpxEncoder..." << std::endl;
}

template <typename T>
std::vector<uint8_t> LibvpxEncoder<T>::encode(Yuv420Frame<T>& frame)
{
    if (sizeof(T) == 1) {
        if (image_.fmt & VPX_IMG_FMT_HIGHBITDEPTH)
            throw std::exception("High-bit depth found in LibvpxEncoder::encode(Yuv420Frame8Bit&).");
    }
    else if (sizeof(T) == 2) {
        if (!(image_.fmt & VPX_IMG_FMT_HIGHBITDEPTH))
            throw std::exception("Low-bit depth found in LibvpxEncoder::encode(Yuv420Frame12Bit&).");
    }
    else {
        throw std::exception("Invalid bit-depth...");
    }

    image_.planes[VPX_PLANE_Y] = reinterpret_cast<unsigned char*>(frame.y_channel_data());
    image_.planes[VPX_PLANE_U] = reinterpret_cast<unsigned char*>(frame.u_channel_data());
    image_.planes[VPX_PLANE_V] = reinterpret_cast<unsigned char*>(frame.v_channel_data());

    image_.stride[VPX_PLANE_Y] = frame.width() * sizeof(T);
    image_.stride[VPX_PLANE_U] = frame.width() * sizeof(T) / 2;
    image_.stride[VPX_PLANE_V] = frame.width() * sizeof(T) / 2;

    int flags = 0;
    if (keyframe_interval_ > 0 && frame_index_ % keyframe_interval_ == 0)
        flags |= VPX_EFLAG_FORCE_KF;

    return encodeFrame(&codec_, &image_, frame_index_++, flags);
}

template <typename T>
std::vector<uint8_t> LibvpxEncoder<T>::encodeFrame(vpx_codec_ctx_t* codec,
    vpx_image_t* image,
    int frame_index,
    int flags)
{
    vpx_codec_iter_t iter = nullptr;
    const vpx_codec_cx_pkt_t* pkt = nullptr;
    const vpx_codec_err_t res = vpx_codec_encode(codec, image, frame_index, 1, flags, VPX_DL_REALTIME);

    if (res != VPX_CODEC_OK) {
        auto error = vpx_codec_error(codec);
        auto error_detail = vpx_codec_error_detail(codec);
        std::cout << "vpx_codec_error: " << error << std::endl;
        std::cout << "vpx_codec_error_detail: " << error_detail << std::endl;
        throw std::exception("Failed to encode frame");
    }

    std::vector<uint8_t> bytes;
    bool found = false;
    while ((pkt = vpx_codec_get_cx_data(codec, &iter)) != nullptr) {
        if (pkt->kind == VPX_CODEC_CX_FRAME_PKT) {
            const int keyframe = (pkt->data.frame.flags & VPX_FRAME_IS_KEY) != 0;
            bytes = std::vector<uint8_t>(pkt->data.frame.sz);
            memcpy(bytes.data(), (uint8_t*)pkt->data.frame.buf, pkt->data.frame.sz);

            if (found)
                throw std::exception("Multiple ByteFrames found.");

            found = true;
        }
    }

    if (!found)
        throw std::exception("No ByteFrames found.");

    return bytes;
}

std::unique_ptr<LibvpxEncoder<uint8_t>> createVp8Encoder(int width, int height, int target_bitrate)
{
    vpx_codec_iface_t* (*const codec_interface)() = &vpx_codec_vp8_cx;
    vpx_codec_enc_cfg_t configuration;

    vpx_codec_err_t res = vpx_codec_enc_config_default(codec_interface(), &configuration, 0);
    if (res != VPX_CODEC_OK)
        throw std::exception("Failed to get default codec config.");

    configuration.g_w = width;
    configuration.g_h = height;
    configuration.rc_target_bitrate = target_bitrate;

    configuration.g_threads = 8; // correct for -threads 8
    configuration.g_lag_in_frames = 0; // correct for -lag-in-frames 0
    configuration.rc_min_quantizer = 4; // correct for -qmin 4
    configuration.rc_max_quantizer = 48; // correct for -qmax 48
    configuration.g_error_resilient = 1;

    vpx_codec_ctx_t codec;
    res = vpx_codec_enc_init(&codec, codec_interface(), &configuration, 0);
    if (res != VPX_CODEC_OK)
        throw std::exception("Failed to initialize encoder");

    int speed = 6;
    vpx_codec_control(&codec, VP8E_SET_CPUUSED, speed);
    int static_threshold = 0;
    vpx_codec_control(&codec, VP8E_SET_STATIC_THRESHOLD, static_threshold);
    int max_intra_rate = 300;
    vpx_codec_control(&codec, VP8E_SET_MAX_INTRA_BITRATE_PCT, max_intra_rate);

    vpx_image_t image;
    if (!vpx_img_alloc(&image, VPX_IMG_FMT_I420, configuration.g_w, configuration.g_h, 32))
        throw std::exception("Failed to allocate image.");

    return std::make_unique<LibvpxEncoder<uint8_t>>(codec, image);
}
}