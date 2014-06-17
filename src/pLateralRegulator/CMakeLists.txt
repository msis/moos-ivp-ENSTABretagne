#--------------------------------------------------------
# The CMakeLists.txt for:                       pRegulator
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  ScalarPID.cpp
  LateralRegulator.cpp
  Regulator_Info.cpp
  Regulator_Tests.cpp
  main.cpp
)



ADD_EXECUTABLE(pLateralRegulator ${SRC})

TARGET_LINK_LIBRARIES(pLateralRegulator
   MOOS
   mbutil
   m
   pthread)

