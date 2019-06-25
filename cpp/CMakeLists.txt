cmake_minimum_required(VERSION 3.13)
project(KinectToHololens)

# To keep asio quite from leaving a lot of warnings.
# reference: https://github.com/chriskohlhoff/asio/issues/290
add_definitions(-D_SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING)

# Assign the top directory as a directory to include.
# This allows including header files without the ".."s.
include_directories("${PROJECT_SOURCE_DIR}/src")

# The path to libfreenect2 included in this repository.
set(LIBFREENECT2_DIR ${PROJECT_SOURCE_DIR}/third_party/libfreenect2-0.2.0-usbdk-vs2015-x64)

# By adding the directory containing asio.hpp, that is the directory containing
# all header files of vcpkg, header files from vcpkg can be included to this project.
find_path(VCPKG_INCLUDE_DIR NAMES asio.hpp PATH_SUFFIXES asio)
include_directories(${VCPKG_INCLUDE_DIR})

# Set ${FFMPEG_LIBRARIES} for FFmpeg.
unset(FFMPEG_LIBRARIES CACHE)
foreach(FFMPEG_SUBLIBRARY avcodec avdevice avfilter avformat avutil swresample swscale)
  find_library(FFMPEG_lib${FFMPEG_SUBLIBRARY}_LIBRARY NAMES ${FFMPEG_SUBLIBRARY})
  list(APPEND FFMPEG_LIBRARIES ${FFMPEG_lib${FFMPEG_SUBLIBRARY}_LIBRARY})
endforeach()

# Set ${OpenCV_LIBS} for OpenCV.
find_package(OpenCV REQUIRED)

# The directory for general source files.
add_subdirectory(src)
# The directory for cpp files corresponding to exe files.
add_subdirectory(examples)
# The directory that contains files required to build the DLL file for Unity. 
add_subdirectory(plugin)