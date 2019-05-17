cmake_minimum_required(VERSION 3.13)
project(KinectToHololens)

# For asio. (reference: https://github.com/chriskohlhoff/asio/issues/290)
add_definitions(-D_SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING)

include_directories("${PROJECT_SOURCE_DIR}/src")

set(LIBFREENECT2_DIR ${PROJECT_SOURCE_DIR}/third_party/libfreenect2-0.2.0-usbdk-vs2015-x64)

# By adding path for asio, the include folder made by vcpkg gets added.
# No need add find_path for other libraries.
find_path(VCPKG_INCLUDE_DIR NAMES asio.hpp PATH_SUFFIXES asio)
include_directories(${VCPKG_INCLUDE_DIR})

set(Libvpx_LIB ${VCPKG_INCLUDE_DIR}/../lib/vpxmd.lib)

# Set FFMPEG_LIBRARIES to link ffmpeg.
unset(FFMPEG_LIBRARIES CACHE)
foreach(FFMPEG_SUBLIBRARY avcodec avdevice avfilter avformat avutil swresample swscale)
  find_library(FFMPEG_lib${FFMPEG_SUBLIBRARY}_LIBRARY NAMES ${FFMPEG_SUBLIBRARY})
  list(APPEND FFMPEG_LIBRARIES ${FFMPEG_lib${FFMPEG_SUBLIBRARY}_LIBRARY})
endforeach()

# Add OpenCV. Use ${OpenCV_LIBS} to link libraries.
find_package(OpenCV REQUIRED)

add_subdirectory(src)
add_subdirectory(examples)
add_subdirectory(plugin)