# This would only work for x64 since freenect2 is only for x64, not x86.

set(SOURCE_FILES
  kinect.h
  kinect.cpp
  raii/kinect_raii.h
  raii/kinect_color_camera_settings.cpp
  raii/kinect_color_frame.cpp
  raii/kinect_depth_frame.cpp
  raii/kinect_body.cpp
  raii/kinect_body_frame.cpp
  raii/kinect_multi_source_frame.cpp
  raii/kinect_multi_source_frame_reader.cpp
  raii/kinect_sensor.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES ${SOURCE_FILES})
add_library(Kinect ${SOURCE_FILES})

target_include_directories(Kinect PRIVATE
  "$ENV{KINECTSDK20_DIR}/inc"
  "${LIBFREENECT2_DIR}/include"
)
target_link_libraries(Kinect
  $ENV{KINECTSDK20_DIR}/lib/x64/Kinect20.lib
  ${LIBFREENECT2_DIR}/lib/freenect2.lib
)
set_target_properties(Kinect PROPERTIES
  CXX_STANDARD 17
)