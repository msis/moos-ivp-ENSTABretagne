#--------------------------------------------------------
# The CMakeLists.txt for:                       pRegulator
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  ScalarPID.cpp
  Regulator.cpp
  Regulator_Info.cpp
  Regulator_Tests.cpp
  main.cpp
)



ADD_EXECUTABLE(pRegulator ${SRC})

TARGET_LINK_LIBRARIES(pRegulator
   MOOS
   mbutil
   m
   pthread)

