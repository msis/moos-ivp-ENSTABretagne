#--------------------------------------------------------
# The CMakeLists.txt for:                       pInertialMeasurementUnit
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)
   
SET(CMAKE_CXX_FLAGS "-D_REENTRANT=1")

SET(SRC
  RazorAHRS/RazorAHRS.cpp
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
