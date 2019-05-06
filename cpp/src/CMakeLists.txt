if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  add_subdirectory(kinect)
endif()

set(SOURCE_FILES
  frames.h
  frames.cpp
  rvl.h
  rvl.cpp
  encoder.h
  encoder.cpp
  decoder.h
  decoder.cpp
  sender.h
  receiver.h
  receiver.cpp
)

add_library(RgbdStreamerLib ${SOURCE_FILES})

target_link_libraries(RgbdStreamerLib
  ${FFMPEG_LIBRARIES}
)
set_target_properties(RgbdStreamerLib PROPERTIES
  CXX_STANDARD 17
)