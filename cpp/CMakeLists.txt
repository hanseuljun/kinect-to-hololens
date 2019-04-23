cmake_minimum_required(VERSION 3.13)
project(rgbd-streamer)

include_directories("${PROJECT_SOURCE_DIR}/src")

set(LIBFREENECT2_DIR ${PROJECT_SOURCE_DIR}/third_party/libfreenect2-0.2.0-usbdk-vs2015-x64)

# Add OpenCV. Use ${OpenCV_LIBS} to link libraries.
find_package(OpenCV REQUIRED)

add_subdirectory(src)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  add_executable(KinectReaderDemo examples/kinect_reader_demo.cpp)
  target_include_directories(KinectReaderDemo PRIVATE
    "$ENV{KINECTSDK20_DIR}/inc"
    "${LIBFREENECT2_DIR}/include"
  )
  target_link_libraries(KinectReaderDemo
    ${OpenCV_LIBS}
    RgbdStreamerLib
    Kinect
  )
  set_target_properties(KinectReaderDemo PROPERTIES
    CXX_STANDARD 17
  )
  add_custom_command(TARGET KinectReaderDemo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${LIBFREENECT2_DIR}/bin/freenect2.dll"
    $<TARGET_FILE_DIR:KinectReaderDemo>
  )
  add_custom_command(TARGET KinectReaderDemo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${LIBFREENECT2_DIR}/bin/glfw3.dll"
    $<TARGET_FILE_DIR:KinectReaderDemo>
  )
  add_custom_command(TARGET KinectReaderDemo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${LIBFREENECT2_DIR}/bin/libusb-1.0.dll"
    $<TARGET_FILE_DIR:KinectReaderDemo>
  )
  add_custom_command(TARGET KinectReaderDemo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${LIBFREENECT2_DIR}/bin/turbojpeg.dll"
    $<TARGET_FILE_DIR:KinectReaderDemo>
  )
endif()