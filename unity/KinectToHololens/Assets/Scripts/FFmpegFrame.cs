using System;

// A class that contains a pointer to a FFmpegFrame in KinectToHololensPlugin.dll.
public class FFmpegFrame
{
    public IntPtr Ptr { get; private set; }

    public FFmpegFrame(IntPtr ptr)
    {
        Ptr = ptr;
    }

    ~FFmpegFrame()
    {
        Plugin.delete_ffmpeg_frame(Ptr);
    }
}
