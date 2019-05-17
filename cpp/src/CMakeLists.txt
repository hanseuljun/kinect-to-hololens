if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  add_subdirectory(kinect)
endif()

set(SOURCE_FILES
  frames.h
  frames.cpp
  rvl.h
  rvl.cpp
  vp8.h
  vp8_encoder.cpp
  vp8_decoder.cpp
  sender.h
  receiver.h
  receiver.cpp
)

add_library(KinectToHololens ${SOURCE_FILES})

target_link_libraries(KinectToHololens
  ${FFMPEG_LIBRARIES}
)
set_target_properties(KinectToHololens PROPERTIES
  CXX_STANDARD 17
)