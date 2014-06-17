#--------------------------------------------------------
# The CMakeLists.txt for:                       pWayPoints
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  WayPoints.cpp
  WayPoints_Info.cpp
  WayPoints_Tests.cpp
  main.cpp
)

ADD_EXECUTABLE(pWayPoints ${SRC})

TARGET_LINK_LIBRARIES(pWayPoints
   MOOS
   MOOSGeodesy
   mbutil
   m
   pthread)

