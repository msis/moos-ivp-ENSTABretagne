#--------------------------------------------------------
# The CMakeLists.txt for:                       iCamera
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  VideoCodec.cpp
  VideoCodec_Info.cpp
  VideoCodec_Tests.cpp
  main.cpp
)

ADD_EXECUTABLE(pVideoCodec ${SRC})

TARGET_LINK_LIBRARIES(pVideoCodec
   MOOS
   mbutil
   m
   pthread
   opencv_core
#   opencv_imgproc
   opencv_highgui
#   opencv_video
)

