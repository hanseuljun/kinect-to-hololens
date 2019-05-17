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
  kh_sender.h
  kh_sender.cpp
  kh_receiver.h
  kh_receiver.cpp
)

add_library(KinectToHololens ${SOURCE_FILES})

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