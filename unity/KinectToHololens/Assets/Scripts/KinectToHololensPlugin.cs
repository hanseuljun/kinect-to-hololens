using System;
using System.Runtime.InteropServices;

public static class KinectToHololensPlugin
{
    private const string DllName = "KinectToHololensPlugin";
    
    [DllImport(DllName)]
    public static extern bool has_unity_interfaces();
}
