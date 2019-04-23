if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  add_subdirectory(kinect)
endif()

set(SOURCE_FILES
  frames.h
  frames.cpp
  rvl.h
  rvl.cpp
)

add_library(RgbdStreamerLib ${SOURCE_FILES})