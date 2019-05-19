using System;
using System.Runtime.InteropServices;

public static class Plugin
{
    private const string DllName = "KinectToHololensPlugin";

    [DllImport(DllName)]
    public static extern bool has_unity_interfaces();

    [DllImport(DllName)]
    public static extern bool has_unity_graphics();

    [DllImport(DllName)]
    public static extern bool has_d3d11_device();

    [DllImport(DllName)]
    public static extern IntPtr get_render_event_function_pointer();

    [DllImport(DllName)]
    public static extern IntPtr texture_group_get_y_texture_view();

    [DllImport(DllName)]
    public static extern IntPtr texture_group_get_u_texture_view();

    [DllImport(DllName)]
    public static extern IntPtr texture_group_get_v_texture_view();

    [DllImport(DllName)]
    public static extern IntPtr texture_group_get_depth_texture_view();

    [DllImport(DllName)]
    public static extern IntPtr texture_group_set_ffmpeg_frame(IntPtr ffmpeg_frame_ptr);

    [DllImport(DllName)]
    public static extern IntPtr texture_group_set_rvl_frame(IntPtr rvl_frame_data, int rvl_frame_size);

    [DllImport(DllName)]
    public static extern IntPtr create_vp8_decoder();

    [DllImport(DllName)]
    public static extern void delete_vp8_decoder(IntPtr ptr);

    [DllImport(DllName)]
    public static extern IntPtr vp8_decoder_decode(IntPtr decoder_ptr, IntPtr frame_ptr, int frame_size);

    [DllImport(DllName)]
    public static extern void delete_ffmpeg_frame(IntPtr ptr);
}
