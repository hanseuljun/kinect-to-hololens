# The 32-bit version of this project is only for the Unity plugin,
# which does not directly relate to Kinects.
# Also, building the source files inside the kinect folder requires
# libfreenect2, but we only have a 64-bit version of it. 
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  add_subdirectory(kinect)
endif()

set(SOURCE_FILES
  kh_core.h
  kh_core.cpp
  kh_vp8.h
  kh_vp8_encoder.cpp
  kh_vp8_decoder.cpp
  kh_rvl.h
  kh_rvl.cpp
  kh_message.h
  kh_message.cpp
  kh_sender.h
  kh_sender.cpp
  kh_receiver.h
  kh_receiver.cpp
)

add_library(KinectToHololens ${SOURCE_FILES})

# These files are not required for the 32-bit version.
# However, they do not prevent the project from getting built
# unless a cpp file related to libfreenect2 gets involved.
target_include_directories(KinectToHololens PRIVATE
  "$ENV{KINECTSDK20_DIR}/inc"
  "${LIBFREENECT2_DIR}/include"
)
target_link_libraries(KinectToHololens
  ${FFMPEG_LIBRARIES}
)
set_target_properties(KinectToHololens PROPERTIES
  CXX_STANDARD 17
)