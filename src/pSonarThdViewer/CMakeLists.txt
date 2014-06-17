#--------------------------------------------------------
# The CMakeLists.txt for:                       pControler
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  SonarThdViewer.cpp
  Controler_Info.cpp
  Controler_Tests.cpp
  main.cpp
)



ADD_EXECUTABLE(pSonarThdViewer ${SRC})

TARGET_LINK_LIBRARIES(pSonarThdViewer
   MOOS
   mbutil
   m
   pthread
   opencv_core
   opencv_imgproc
   opencv_highgui
   )

