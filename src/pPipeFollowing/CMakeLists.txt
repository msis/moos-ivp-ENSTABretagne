#--------------------------------------------------------
# The CMakeLists.txt for:                       pPipeFollowing
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  PipeFollowing.cpp
  PipeFollowing_Info.cpp
  PipeFollowing_Tests.cpp
  main.cpp
)

SET(SRC_LINREG
  regression_lineaire/linreg.cpp
)

ADD_EXECUTABLE(pPipeFollowing ${SRC} ${SRC_LINREG})

TARGET_LINK_LIBRARIES(pPipeFollowing
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

