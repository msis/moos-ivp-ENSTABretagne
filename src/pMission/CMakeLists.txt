#--------------------------------------------------------
# The CMakeLists.txt for:                       pMission
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  Mission.cpp
  Mission_Info.cpp
  Mission_Tests.cpp
  main.cpp
)



ADD_EXECUTABLE(pMission ${SRC})

TARGET_LINK_LIBRARIES(pMission
   MOOS
   mbutil
   m
   pthread)

