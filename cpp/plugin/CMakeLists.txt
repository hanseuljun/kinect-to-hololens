set(SOURCE_FILES
  plugin.def
  plugin.cpp
  kh_plugin.cpp
  texture_group.h
  texture_group.cpp
  channel_texture.h
  channel_texture.cpp
  depth_texture.h
  depth_texture.cpp
  unity/IUnityGraphics.h
  unity/IUnityGraphicsD3D11.h
  unity/IUnityInterface.h
)

add_library(KinectToHololensPlugin SHARED
  ${SOURCE_FILES}
)
target_link_libraries(KinectToHololensPlugin
  KinectToHololens
)
set_target_properties(KinectToHololensPlugin PROPERTIES
  CXX_STANDARD 17
)