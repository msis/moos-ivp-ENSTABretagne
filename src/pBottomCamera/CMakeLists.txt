#--------------------------------------------------------
# The CMakeLists.txt for:                       pBottomCamera
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  BottomCamera.cpp
  BottomCamera_Info.cpp
  BottomCamera_Tests.cpp
  main.cpp
)



ADD_EXECUTABLE(pBottomCamera ${SRC})

TARGET_LINK_LIBRARIES(pBottomCamera
   MOOS
   mbutil
   m
   pthread)

