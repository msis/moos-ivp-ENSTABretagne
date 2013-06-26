#--------------------------------------------------------
# The CMakeLists.txt for:                       iInertialMeasurementUnit
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



ADD_EXECUTABLE(iInertialMeasurementUnit ${SRC})

TARGET_LINK_LIBRARIES(iInertialMeasurementUnit
   MOOS
   mbutil
   m
   pthread)
