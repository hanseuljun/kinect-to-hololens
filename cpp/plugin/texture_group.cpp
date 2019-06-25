#include "texture_group.h"

#include "unity/IUnityInterface.h"
#include "depth_texture.h"
#include "channel_texture.h"

typedef void* VoidPtr;

// COLOR_WIDTH and COLOR_HEIGHT are half of the ones of Kinect since color resolution gets halved
// before getting sent by a Sender.
// DEPTH_WIDTH and DEPTH_HEIGHT are for the resolution of the Kinect depth camera.
const int COLOR_WIDTH = 960;
const int COLOR_HEIGHT = 540;
const int DEPTH_WIDTH = 512;
const int DEPTH_HEIGHT = 424;

// Instances of classes for Direct3D textures.
std::unique_ptr<kh::ChannelTexture> y_texture_;
std::unique_ptr<kh::ChannelTexture> u_texture_;
std::unique_ptr<kh::ChannelTexture> v_texture_;
std::unique_ptr<kh::DepthTexture> depth_texture_;

// Unity connects Unity textures to Direct3D textures through creating Unity textures binded to these texture views.
ID3D11ShaderResourceView* y_texture_view_ = nullptr;
ID3D11ShaderResourceView* u_texture_view_ = nullptr;
ID3D11ShaderResourceView* v_texture_view_ = nullptr;
ID3D11ShaderResourceView* depth_texture_view_ = nullptr;

// These variables get set in the main thread of Unity, then gets assigned to textures in a render thread of Unity.
kh::FFmpegFrame ffmpeg_frame_(nullptr);
std::vector<uint8_t> rvl_frame_;

// A function that intializes Direct3D resources. Should be called in a render thread.
void texture_group_init(ID3D11Device* device)
{
    y_texture_ = std::make_unique<kh::ChannelTexture>(device, COLOR_WIDTH, COLOR_HEIGHT);
    u_texture_ = std::make_unique<kh::ChannelTexture>(device, COLOR_WIDTH / 2, COLOR_HEIGHT / 2);
    v_texture_ = std::make_unique<kh::ChannelTexture>(device, COLOR_WIDTH / 2, COLOR_HEIGHT / 2);
    depth_texture_ = std::make_unique<kh::DepthTexture>(device, DEPTH_WIDTH, DEPTH_HEIGHT);

    // Set the texture view variables, so Unity can create Unity textures that are connected to the textures through the texture views.
    y_texture_view_ = y_texture_->getTextureView(device);
    u_texture_view_ = u_texture_->getTextureView(device);
    v_texture_view_ = v_texture_->getTextureView(device);
    depth_texture_view_ = depth_texture_->getTextureView(device);
}

extern "C" VoidPtr UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API texture_group_get_y_texture_view()
{
    return y_texture_view_;
}

extern "C" VoidPtr UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API texture_group_get_u_texture_view()
{
    return u_texture_view_;
}

extern "C" VoidPtr UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API texture_group_get_v_texture_view()
{
    return v_texture_view_;
}

extern "C" VoidPtr UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API texture_group_get_depth_texture_view()
{
    return depth_texture_view_;
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API texture_group_set_ffmpeg_frame(void* ffmpeg_frame_ptr)
{
    auto ffmpeg_frame = reinterpret_cast<kh::FFmpegFrame*>(ffmpeg_frame_ptr);
    ffmpeg_frame_ = std::move(*ffmpeg_frame);
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API texture_group_set_rvl_frame(void* rvl_frame_data, int rvl_frame_size)
{
    rvl_frame_ = std::vector<uint8_t>(rvl_frame_size);
    memcpy(rvl_frame_.data(), rvl_frame_data, rvl_frame_size);
}

// Updating pixels of the textures. Should be called in a render thread.
void texture_group_update(ID3D11Device* device, ID3D11DeviceContext* device_context)
{
    y_texture_->updatePixels(device, device_context, COLOR_WIDTH, COLOR_HEIGHT, ffmpeg_frame_, 0);
    u_texture_->updatePixels(device, device_context, COLOR_WIDTH / 2, COLOR_HEIGHT / 2, ffmpeg_frame_, 1);
    v_texture_->updatePixels(device, device_context, COLOR_WIDTH / 2, COLOR_HEIGHT / 2, ffmpeg_frame_, 2);
    depth_texture_->updatePixels(device, device_context, DEPTH_WIDTH, DEPTH_HEIGHT, rvl_frame_);
}