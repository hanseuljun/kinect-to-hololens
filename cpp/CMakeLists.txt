cmake_minimum_required(VERSION 3.13)
project(rgbd-streamer)

include_directories("${PROJECT_SOURCE_DIR}/src")

set(LIBFREENECT2_DIR ${PROJECT_SOURCE_DIR}/third_party/libfreenect2-0.2.0-usbdk-vs2015-x64)

# By adding path for asio, the include folder made by vcpkg gets added.
# No need add find_path for other libraries.
find_path(VCPKG_INCLUDE_DIR NAMES asio.hpp PATH_SUFFIXES asio)
include_directories(${VCPKG_INCLUDE_DIR})

set(Libvpx_LIB ${VCPKG_INCLUDE_DIR}/../lib/vpxmd.lib)

# Add OpenCV. Use ${OpenCV_LIBS} to link libraries.
find_package(OpenCV REQUIRED)

add_subdirectory(src)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  add_executable(KinectReaderDemo
    examples/kinect_reader_demo.cpp
    examples/helper/opencv_helper.h
  )
  target_include_directories(KinectReaderDemo PRIVATE
    "$ENV{KINECTSDK20_DIR}/inc"
    "${LIBFREENECT2_DIR}/include"
  )
  target_link_libraries(KinectReaderDemo
    ${OpenCV_LIBS}
    RgbdStreamerLib
    Kinect
    ${Libvpx_LIB}
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