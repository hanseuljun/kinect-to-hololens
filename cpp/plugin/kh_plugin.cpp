
#include "unity/IUnityInterface.h"
#include "kh_vp8.h"

typedef void* VoidPtr;

// External functions for Unity C# scripts.
extern "C" VoidPtr UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API create_vp8_decoder()
{
    return new kh::Vp8Decoder;
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API delete_vp8_decoder(void* ptr)
{
    delete reinterpret_cast<kh::Vp8Decoder*>(ptr);
}

extern "C" VoidPtr UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API vp8_decoder_decode
(
    void* decoder_ptr,
    void* frame_ptr,
    int frame_size
)
{
    auto decoder = reinterpret_cast<kh::Vp8Decoder*>(decoder_ptr);
    auto frame_data = reinterpret_cast<uint8_t*>(frame_ptr);
    return new kh::FFmpegFrame(std::move(decoder->decode(frame_data, frame_size)));
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API delete_ffmpeg_frame(void* ptr)
{
    delete reinterpret_cast<kh::FFmpegFrame*>(ptr);
}