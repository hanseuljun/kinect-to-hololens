#pragma once

#include <vector>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavutil/frame.h>
}

namespace kh
{
// Data of this class is not supposed to be copy since it will take a lot of computational resource.
class YuvImage
{
public:
    YuvImage(std::vector<uint8_t>&& y_channel, std::vector<uint8_t>&& u_channel,
        std::vector<uint8_t>&& v_channel, int width, int height)
        : y_channel_(std::move(y_channel))
        , u_channel_(std::move(u_channel))
        , v_channel_(std::move(v_channel))
        , width_(width)
        , height_(height)
    {
    }
    YuvImage(const YuvImage& other) = delete;
    YuvImage& operator=(const YuvImage& other) = delete;
    YuvImage(YuvImage&& other) noexcept
        : y_channel_(std::move(other.y_channel_))
        , u_channel_(std::move(other.u_channel_))
        , v_channel_(std::move(other.v_channel_))
        , width_(other.width_)
        , height_(other.height_)
    {
    }
    YuvImage& operator=(YuvImage&& other) noexcept
    {
        y_channel_ = std::move(other.y_channel_);
        u_channel_ = std::move(other.u_channel_);
        v_channel_ = std::move(other.v_channel_);
        width_ = other.width_;
        height_ = other.height_;
        return *this;
    }
    std::vector<uint8_t>& y_channel() { return y_channel_; }
    std::vector<uint8_t>& u_channel() { return u_channel_; }
    std::vector<uint8_t>& v_channel() { return v_channel_; }
    int width() const { return width_; }
    int height() const { return height_; }

private:
    std::vector<uint8_t> y_channel_;
    std::vector<uint8_t> u_channel_;
    std::vector<uint8_t> v_channel_;
    int width_;
    int height_;
};

class FFmpegFrame
{
public:
    FFmpegFrame(AVFrame* av_frame)
        : av_frame_(av_frame)
    {
    }
    ~FFmpegFrame()
    {
        if (av_frame_)
            av_frame_free(&av_frame_);
    }
    FFmpegFrame(const FFmpegFrame& other) = delete;
    FFmpegFrame& operator=(const FFmpegFrame& other) = delete;
    FFmpegFrame(FFmpegFrame&& other) noexcept
        : av_frame_(other.av_frame_)
    {
        other.av_frame_ = nullptr;
    }
    FFmpegFrame& operator=(FFmpegFrame&& other) noexcept
    {
        if (av_frame_)
            av_frame_free(&av_frame_);

        av_frame_ = other.av_frame_;
        other.av_frame_ = nullptr;
        return *this;
    }
    AVFrame* av_frame() { return av_frame_; }

private:
    AVFrame* av_frame_;
};

// Downsample width and height by 2.
YuvImage createHalvedYuvImageFromKinectColorBuffer(uint8_t* buffer);
YuvImage createYuvImageFromAvFrame(AVFrame* av_frame);
std::vector<uint8_t> createRvlFrameFromKinectDepthBuffer(uint16_t* buffer);
std::vector<uint16_t> createDepthImageFromRvlFrame(uint8_t* rvl_frame);
}