#--------------------------------------------------------
# The CMakeLists.txt for:                       uMapping
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  Mapping.cpp
  Mapping_Info.cpp
  Mapping_Tests.cpp
  main.cpp
)



ADD_EXECUTABLE(uMapping ${SRC})

TARGET_LINK_LIBRARIES(uMapping
   MOOS
   mbutil
   m
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
   opencv_flann
   opencv_stitching
   opencv_nonfree
    )
