#pragma once

#include <vector>

namespace rgbd_streamer
{
// Data of this class is not supposed to be copy since it will take a lot of computational resource.
template<typename T>
class Yuv420Frame
{
public:
    Yuv420Frame(std::vector<T>&& y_channel,
        std::vector<T>&& u_channel,
        std::vector<T>&& v_channel,
        int width,
        int height)
        : y_channel_(std::move(y_channel))
        , u_channel_(std::move(u_channel))
        , v_channel_(std::move(v_channel))
        , width_(width)
        , height_(height)
    {
    }
    Yuv420Frame(const Yuv420Frame& other) = delete;
    Yuv420Frame& operator=(const Yuv420Frame& other) = delete;
    Yuv420Frame(Yuv420Frame&& other) noexcept = default;
    Yuv420Frame& operator=(Yuv420Frame&& other) noexcept = default;
    const std::vector<T>& y_channel() const { return y_channel_; }
    const std::vector<T>& u_channel() const { return u_channel_; }
    const std::vector<T>& v_channel() const { return v_channel_; }
    T* y_channel_data() { return y_channel_.data(); }
    T* u_channel_data() { return u_channel_.data(); }
    T* v_channel_data() { return v_channel_.data(); }
    int width() const { return width_; }
    int height() const { return height_; }

private:
    std::vector<T> y_channel_;
    std::vector<T> u_channel_;
    std::vector<T> v_channel_;
    int width_;
    int height_;
};
typedef Yuv420Frame<uint8_t> Yuv420ByteFrame;
typedef Yuv420Frame<uint16_t> Yuv420DoubleByteFrame;

Yuv420ByteFrame createYuv420ByteFrameFromKinectColorFrame(uint8_t* buffer);
// Downsample width and height by 2.
Yuv420ByteFrame createDownsampledYuv420ByteFrameFromKinectColorFrame(uint8_t* buffer);
std::vector<uint8_t> createRvlFrameFromKinectDepthFrame(uint16_t* buffer);
std::vector<uint16_t> createDepthFrameFromRvlFrame(uint8_t* rvl_frame);
}