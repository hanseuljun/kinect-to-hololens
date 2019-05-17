set(SOURCE_FILES
  plugin.def
  plugin.cpp
  unity/IUnityGraphics.h
  unity/IUnityGraphicsD3D11.h
  unity/IUnityInterface.h
)

add_library(KinectToHololensPlugin SHARED
  ${SOURCE_FILES}
)
set_target_properties(KinectToHololensPlugin PROPERTIES
  CXX_STANDARD 17
)