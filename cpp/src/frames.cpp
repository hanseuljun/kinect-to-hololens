#include "frames.h"

#include "rvl.h"

namespace rgbd_streamer
{
YuvFrame createYuvFrameFromKinectColorBuffer(uint8_t* buffer)
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

    return YuvFrame(std::move(y_channel), std::move(u_channel), std::move(v_channel), WIDTH, HEIGHT);
}

// Downsample width and height by 2.
YuvFrame createHalfSizedYuvFrameFromKinectColorBuffer(uint8_t* buffer)
{
    // The width and height of Kinect's color frames.
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

    return YuvFrame(std::move(y_channel), std::move(u_channel), std::move(v_channel), DOWNSAMPLED_WIDTH, DOWNSAMPLED_HEIGHT);
}

std::vector<uint8_t> convertPicturePlaneToBytes(uint8_t* data, int line_size, int width, int height)
{
    std::vector<uint8_t> bytes(width * height);
    for (int i = 0; i < height; ++i)
        memcpy(bytes.data() + i * width, data + i * line_size, width);

    return bytes;
}

YuvFrame createYuvFrameFromAvFrame(AVFrame* av_frame)
{
    return YuvFrame(
        std::move(convertPicturePlaneToBytes(av_frame->data[0], av_frame->linesize[0], av_frame->width, av_frame->height)),
        std::move(convertPicturePlaneToBytes(av_frame->data[1], av_frame->linesize[1], av_frame->width / 2, av_frame->height / 2)),
        std::move(convertPicturePlaneToBytes(av_frame->data[2], av_frame->linesize[2], av_frame->width / 2, av_frame->height / 2)),
        av_frame->width,
        av_frame->height);
}

std::vector<uint8_t> createRvlFrameFromKinectDepthBuffer(uint16_t* buffer)
{
    // The width and height of Kinect's depth frames.
    const int WIDTH = 512;
    const int HEIGHT = 424;

    return rvl::compress(buffer, WIDTH * HEIGHT);
}

std::vector<uint16_t> createDepthFrameFromRvlFrame(uint8_t* rvl_frame)
{
    // The width and height of Kinect's depth frames.
    const int WIDTH = 512;
    const int HEIGHT = 424;

    return rvl::decompress(rvl_frame, WIDTH * HEIGHT);
}
}