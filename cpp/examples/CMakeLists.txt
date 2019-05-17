if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  add_executable(KinectReaderDemo
    kinect_reader_demo.cpp
    helper/opencv_helper.h
  )
  target_include_directories(KinectReaderDemo PRIVATE
    "$ENV{KINECTSDK20_DIR}/inc"
    "${LIBFREENECT2_DIR}/include"
  )
  target_link_libraries(KinectReaderDemo
    ${OpenCV_LIBS}
    KinectToHololens
    Kinect
    ${Libvpx_LIB}
  )
  set_target_properties(KinectReaderDemo PROPERTIES
    CXX_STANDARD 17
  )
  add_custom_command(TARGET KinectReaderDemo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${LIBFREENECT2_DIR}/bin/freenect2.dll"
    $<TARGET_FILE_DIR:KinectReaderDemo>
  )
  add_custom_command(TARGET KinectReaderDemo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${LIBFREENECT2_DIR}/bin/glfw3.dll"
    $<TARGET_FILE_DIR:KinectReaderDemo>
  )
  add_custom_command(TARGET KinectReaderDemo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${LIBFREENECT2_DIR}/bin/libusb-1.0.dll"
    $<TARGET_FILE_DIR:KinectReaderDemo>
  )
  add_custom_command(TARGET KinectReaderDemo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${LIBFREENECT2_DIR}/bin/turbojpeg.dll"
    $<TARGET_FILE_DIR:KinectReaderDemo>
  )

  add_executable(KinectSenderDemo
    kinect_sender_demo.cpp
  )
  target_include_directories(KinectSenderDemo PRIVATE
    "$ENV{KINECTSDK20_DIR}/inc"
    "${LIBFREENECT2_DIR}/include"
  )
  target_link_libraries(KinectSenderDemo
    KinectToHololens
    Kinect
    ${Libvpx_LIB}
  )
  set_target_properties(KinectSenderDemo PROPERTIES
    CXX_STANDARD 17
  )
  add_custom_command(TARGET KinectSenderDemo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${LIBFREENECT2_DIR}/bin/freenect2.dll"
    $<TARGET_FILE_DIR:KinectSenderDemo>
  )
  add_custom_command(TARGET KinectSenderDemo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${LIBFREENECT2_DIR}/bin/glfw3.dll"
    $<TARGET_FILE_DIR:KinectSenderDemo>
  )
  add_custom_command(TARGET KinectSenderDemo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${LIBFREENECT2_DIR}/bin/libusb-1.0.dll"
    $<TARGET_FILE_DIR:KinectSenderDemo>
  )
  add_custom_command(TARGET KinectSenderDemo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${LIBFREENECT2_DIR}/bin/turbojpeg.dll"
    $<TARGET_FILE_DIR:KinectSenderDemo>
  )
endif()

add_executable(KinectReceiverDemo
  kinect_receiver_demo.cpp
)
target_link_libraries(KinectReceiverDemo
  ${OpenCV_LIBS}
  KinectToHololens
)
set_target_properties(KinectReceiverDemo PROPERTIES
  CXX_STANDARD 17
)