#--------------------------------------------------------
# The CMakeLists.txt for:                       pInertialMeasurementUnit
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  InertialMeasurementUnit.cpp
  InertialMeasurementUnit_Info.cpp
  InertialMeasurementUnit_Tests.cpp
  main.cpp
)



ADD_EXECUTABLE(pInertialMeasurementUnit ${SRC})

TARGET_LINK_LIBRARIES(pInertialMeasurementUnit
   MOOS
   mbutil
   m
   pthread)

