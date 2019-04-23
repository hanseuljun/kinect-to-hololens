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

Yuv420ByteFrame createYuv420ByteFromKinectColorFrame(uint8_t* buffer)
{
    // The width and height of Kinect's color frames.
    const int WIDTH = 1920;
    const int HEIGHT = 1080;

    // assumes ColorImageFormat_Yuy2
    std::vector<uint8_t> y_channel(WIDTH * HEIGHT);
    std::vector<uint8_t> u_channel(WIDTH * HEIGHT / 4);
    std::vector<uint8_t> v_channel(WIDTH * HEIGHT / 4);

    for (int i = 0; i < WIDTH * HEIGHT; ++i)
        y_channel[i] = buffer[i * 2];

    const int uv_width = WIDTH / 2;
    const int uv_height = HEIGHT / 2;
    for (int i = 0; i < uv_width; ++i) {
        for (int j = 0; j < uv_height; ++j) {
            int ii = i * 2;
            int jj = j * 2;
            int uv_index = i + j * (WIDTH / 2);
            int buffer_index = (ii + WIDTH * jj) * 2 + 1;
            u_channel[uv_index] = (buffer[buffer_index] + buffer[buffer_index + WIDTH * 2]) / 2;
            v_channel[uv_index] = (buffer[buffer_index + 1 * 2] + buffer[buffer_index + (1 + WIDTH) * 2]) / 2;
        }
    }

    return Yuv420ByteFrame(std::move(y_channel), std::move(u_channel), std::move(v_channel), WIDTH, HEIGHT);
}

// Downsample width and height by 2.
Yuv420ByteFrame createDownsampledYuv420ByteFromKinectColorFrame(uint8_t* buffer)
{
    const int WIDTH = 1920;
    const int HEIGHT = 1080;

    const int DOWNSAMPLED_WIDTH = WIDTH / 2;
    const int DOWNSAMPLED_HEIGHT = HEIGHT / 2;

    // assumes ColorImageFormat_Yuy2
    std::vector<uint8_t> y_channel(DOWNSAMPLED_WIDTH * DOWNSAMPLED_HEIGHT);
    std::vector<uint8_t> u_channel(DOWNSAMPLED_WIDTH * DOWNSAMPLED_HEIGHT / 4);
    std::vector<uint8_t> v_channel(DOWNSAMPLED_WIDTH * DOWNSAMPLED_HEIGHT / 4);

    int y_channel_index = 0;
    for (int j = 0; j < DOWNSAMPLED_HEIGHT; ++j) {
        int buffer_index = j * WIDTH * 4;
        for (int i = 0; i < DOWNSAMPLED_WIDTH; ++i) {
            y_channel[y_channel_index++] = buffer[buffer_index];
            buffer_index += 4;
        }
    }

    int uv_width = WIDTH / 2;
    int uv_height = HEIGHT / 2;
    int downsampled_uv_width = uv_width / 2;
    int downsampled_uv_height = uv_height / 2;

    int uv_index = 0;
    for (int j = 0; j < downsampled_uv_height; ++j) {
        int buffer_index = j * WIDTH * 8 + 1;
        for (int i = 0; i < downsampled_uv_width; ++i) {
            u_channel[uv_index] = buffer[buffer_index];
            v_channel[uv_index] = buffer[buffer_index + 2];
            ++uv_index;
            buffer_index += 8;
        }
    }

    return Yuv420ByteFrame(std::move(y_channel), std::move(u_channel), std::move(v_channel), DOWNSAMPLED_WIDTH, DOWNSAMPLED_HEIGHT);
}
}