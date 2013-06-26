#--------------------------------------------------------
# The CMakeLists.txt for:                       iCiscrea
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  AUV.cpp
  Matrice.cpp
  Propulseur.cpp
  Ciscrea.cpp
  Ciscrea_Info.cpp
  Ciscrea_Tests.cpp
  M6dbus.cpp
  main.cpp
)

ADD_EXECUTABLE(iCiscrea ${SRC})

TARGET_LINK_LIBRARIES(iCiscrea
   MOOS
   mbutil
   m
   modbus
   pthread
   opencv_core
   opencv_imgproc
   opencv_highgui
   opencv_ml
   opencv_video
   opencv_features2d
   opencv_calib3d
   opencv_objdetect
   opencv_contrib
   opencv_legacy
   opencv_flann)