namespace rgbd_streamer
{
namespace internal
{
cv::Mat convertBytesToMat(int width, int height, const std::vector<uint8_t>& v)
{
    cv::Mat mat(height, width, CV_8UC1);
    memcpy(mat.data, v.data(), width * height);
    return mat;
}
} // end of namespace internal

cv::Mat convertYuv420ByteFrameToBgrMat(const Yuv420ByteFrame& frame)
{
    cv::Mat y_channel = internal::convertBytesToMat(frame.width(), frame.height(), frame.y_channel());
    cv::Mat u_channel = internal::convertBytesToMat(frame.width() / 2, frame.height() / 2, frame.u_channel());
    cv::Mat v_channel = internal::convertBytesToMat(frame.width() / 2, frame.height() / 2, frame.v_channel());
    cv::Mat cr_channel;
    cv::Mat cb_channel;
    // u and v corresponds to Cb and Cr
    cv::resize(v_channel, cr_channel, cv::Size(v_channel.cols * 2, v_channel.rows * 2));
    cv::resize(u_channel, cb_channel, cv::Size(u_channel.cols * 2, u_channel.rows * 2));

    std::vector<cv::Mat> y_cr_cb_channels;
    y_cr_cb_channels.push_back(y_channel);
    y_cr_cb_channels.push_back(cr_channel);
    y_cr_cb_channels.push_back(cb_channel);

    cv::Mat y_cr_cb_frame;
    cv::merge(y_cr_cb_channels, y_cr_cb_frame);

    cv::Mat bgr_frame = y_cr_cb_frame.clone();
    cvtColor(y_cr_cb_frame, bgr_frame, CV_YCrCb2BGR);
    return bgr_frame;
}

cv::Mat convertKinectDepthFrameToBgrMat(const std::vector<uint16_t> depth_frame)
{
    const int WIDTH = 512;
    const int HEIGHT = 424;

    std::vector<uint8_t> reduced_depth_frame(WIDTH * HEIGHT);
    std::vector<uint8_t> half(WIDTH * HEIGHT);

    for (int i = 0; i < WIDTH * HEIGHT; ++i) {
        reduced_depth_frame[i] = depth_frame[i] / 16;
        half[i] = 128;
    }

    cv::Mat y_channel(HEIGHT, WIDTH, CV_8UC1, reduced_depth_frame.data());
    cv::Mat cr_channel(HEIGHT, WIDTH, CV_8UC1, half.data());
    cv::Mat cb_channel(HEIGHT, WIDTH, CV_8UC1, half.data());

    std::vector<cv::Mat> y_cr_cb_channels;
    y_cr_cb_channels.push_back(y_channel);
    y_cr_cb_channels.push_back(cr_channel);
    y_cr_cb_channels.push_back(cb_channel);

    cv::Mat y_cr_cb_frame;
    cv::merge(y_cr_cb_channels, y_cr_cb_frame);

    cv::Mat bgr_frame = y_cr_cb_frame.clone();
    cvtColor(y_cr_cb_frame, bgr_frame, CV_YCrCb2BGR);
    return bgr_frame;
}
}