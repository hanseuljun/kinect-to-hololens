using System;

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
