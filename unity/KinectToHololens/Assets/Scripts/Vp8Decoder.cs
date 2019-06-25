using System;

// A class that contains a pointer to a Vp8Decoder in KinectToHololensPlugin.dll.
public class Vp8Decoder
{
    private IntPtr ptr;

    public Vp8Decoder()
    {
        ptr = Plugin.create_vp8_decoder();
    }

    ~Vp8Decoder()
    {
        Plugin.delete_vp8_decoder(ptr);
    }

    public FFmpegFrame Decode(IntPtr framePtr, int frameSize)
    {
        return new FFmpegFrame(Plugin.vp8_decoder_decode(ptr, framePtr, frameSize));
    }
}
