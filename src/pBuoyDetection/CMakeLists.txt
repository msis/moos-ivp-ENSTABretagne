#--------------------------------------------------------
# The CMakeLists.txt for:                       pBuoyDetection
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  BuoyDetection.cpp
  BuoyDetection_Info.cpp
  BuoyDetection_Tests.cpp
  main.cpp
)

ADD_EXECUTABLE(pBuoyDetection ${SRC})

TARGET_LINK_LIBRARIES(pBuoyDetection
   MOOS
   mbutil
   m
   pthread
   opencv_core
   opencv_imgproc
   opencv_highgui
#   opencv_ml
#   opencv_video
#   opencv_features2d
#   opencv_calib3d
#   opencv_objdetect
#   opencv_contrib
#   opencv_legacy
#   opencv_flann
)

