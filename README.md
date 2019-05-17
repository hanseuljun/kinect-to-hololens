# kinect-to-hololens
Receiving RGBD pixels from a Kinect v2 in 30Hz and rendering them in 60 Hz as quads with a HoloLens 1.

# Requirement
A Windows 10 computer with CMake, Visual Studio 2017, and Unity3D 2018.4; a Kinect v2; and a HoloLens 1.

# How to build using source files
1. Initiate vcpkg as a submodule (git submodule init).
2. Run bootstrap-vcpkg.bat in vcpkg to build vcpkg.exe.
3. Use vcpkg.exe to install c++ libraries: asio, ffmpeg, libvpx, and opencv.
- .\vcpkg.exe install asio:x86-windows asio:x64-windows ffmpeg:x86-windows ffmpeg:x64-windows libvpx:x86-windows libvpx:x64-windows opencv:x86-windows opencv:x64-windows
4. Run run-cmake.ps1 in cpp to build Visual Studio solutions using CMake.
5. Run build-plugin.ps1 to build a Unity3D plugin with the Visual Studio solutions.
6. Run copy-dlls.ps1 to copy DLL files including the plugin built in the previous step for the Unity3D project in unity.
7. Build executable files with the Unity3D project and the Visual Studio solutions in cpp/build/x64.

# How to run
## kinect_reader_demo.exe
- Connect your computer to a Kinect v2.
- Run the exe file.

## kinect_sender_demo.exe and kinect_receiver_demo.exe
- Connect a computer to a Kinect v2.
- Run kinect_sender_demo.exe and enter a port number.
- Run kinect_receiver_demo.exe from another computer and enter the IP address and the port number of the sender.