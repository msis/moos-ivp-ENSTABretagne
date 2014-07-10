#--------------------------------------------------------
# The CMakeLists.txt for:                       pWayPointsViewer
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  WayPointsViewer.cpp
  WayPointsViewer_Info.cpp
  WayPointsViewer_Tests.cpp
  main.cpp
)

ADD_EXECUTABLE(pWayPointsViewer ${SRC})

TARGET_LINK_LIBRARIES(pWayPointsViewer
   MOOS
   MOOSGeodesy
   proj
   mbutil
   m
   pthread
   opencv_core
   opencv_imgproc
   opencv_highgui
)

