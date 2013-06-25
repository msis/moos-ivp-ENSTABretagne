#--------------------------------------------------------
# The CMakeLists.txt for:                       pWallFollowing
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  WallFollowing.cpp
  WallFollowing_Info.cpp
  WallFollowing_Tests.cpp
  main.cpp
)



ADD_EXECUTABLE(pWallFollowing ${SRC})

TARGET_LINK_LIBRARIES(pWallFollowing
   MOOS
   mbutil
   m
   pthread)

