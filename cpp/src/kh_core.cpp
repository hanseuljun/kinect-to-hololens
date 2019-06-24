#include "kh_core.h"

#include "kh_rvl.h"

namespace kh
{
// Receives color pixels from the Kinect and converts it into a frame formatted Yuv420 for Vp8Encoder,
// downsampling width and height by 2.
YuvImage createHalvedYuvImageFromKinectColorBuffer(uint8_t* buffer)
{
    // The width and height of Kinect's color frames.
    const int WIDTH = 1920;
    const int HEIGHT = 1080;

    const int DOWNSAMPLED_WIDTH = WIDTH / 2;
    const int DOWNSAMPLED_HEIGHT = HEIGHT / 2;

    // Sizes assume Kinect runs in ColorImageFormat_Yuy2.
    std::vector<uint8_t> y_channel(DOWNSAMPLED_WIDTH * DOWNSAMPLED_HEIGHT);
    std::vector<uint8_t> u_channel(DOWNSAMPLED_WIDTH * DOWNSAMPLED_HEIGHT / 4);
    std::vector<uint8_t> v_channel(DOWNSAMPLED_WIDTH * DOWNSAMPLED_HEIGHT / 4);

    // Conversion of the Y channels of the pixels.
    int y_channel_index = 0;
    for (int j = 0; j < DOWNSAMPLED_HEIGHT; ++j) {
        int buffer_index = j * WIDTH * 4;
        for (int i = 0; i < DOWNSAMPLED_WIDTH; ++i) {
            y_channel[y_channel_index++] = buffer[buffer_index];
            buffer_index += 4;
        }
    }

    // Calculation of the U and V channels of the pixels.
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

    return YuvImage(std::move(y_channel), std::move(u_channel), std::move(v_channel), DOWNSAMPLED_WIDTH, DOWNSAMPLED_HEIGHT);
}

// A helper function for createYuvImageFromAvFrame that converts a AVFrame into a std::vector.
std::vector<uint8_t> convertPicturePlaneToBytes(uint8_t* data, int line_size, int width, int height)
{
    std::vector<uint8_t> bytes(width * height);
    for (int i = 0; i < height; ++i)
        memcpy(bytes.data() + i * width, data + i * line_size, width);

    return bytes;
}

// Converts an outcome of Vp8Deocder into YuvImage so it can be converted for OpenCV with createCvMatFromYuvImage().
YuvImage createYuvImageFromAvFrame(AVFrame* av_frame)
{
    return YuvImage(
        std::move(convertPicturePlaneToBytes(av_frame->data[0], av_frame->linesize[0], av_frame->width, av_frame->height)),
        std::move(convertPicturePlaneToBytes(av_frame->data[1], av_frame->linesize[1], av_frame->width / 2, av_frame->height / 2)),
        std::move(convertPicturePlaneToBytes(av_frame->data[2], av_frame->linesize[2], av_frame->width / 2, av_frame->height / 2)),
        av_frame->width,
        av_frame->height);
}

// A helper function for rvl::compress that also adds consistency through our codebase
// since it is similar to createHalvedYuvImageFromKinectColorBuffer().
std::vector<uint8_t> createRvlFrameFromKinectDepthBuffer(uint16_t* buffer)
{
    // The width and height of Kinect's depth frames.
    const int WIDTH = 512;
    const int HEIGHT = 424;

    return rvl::compress(buffer, WIDTH * HEIGHT);
}

// A helper function for rvl::decompress that also adds consistency through our codebase
// since it is similar to createHalvedYuvImageFromKinectColorBuffer().
std::vector<uint16_t> createDepthImageFromRvlFrame(uint8_t* rvl_frame)
{
    // The width and height of Kinect's depth frames.
    const int WIDTH = 512;
    const int HEIGHT = 424;

    return rvl::decompress(rvl_frame, WIDTH * HEIGHT);
}
}