#--------------------------------------------------------
# The CMakeLists.txt for:                       pAroundTheIsland
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  AroundTheIsland.cpp
  AroundTheIsland_Info.cpp
  AroundTheIsland_Tests.cpp
  main.cpp
)

ADD_EXECUTABLE(pAroundTheIsland ${SRC})

TARGET_LINK_LIBRARIES(pAroundTheIsland
   MOOS
   mbutil
   m
   pthread)

