#--------------------------------------------------------
# The CMakeLists.txt for:                       iTemperatureCPU
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  TemperatureCPU.cpp
  TemperatureCPU_Info.cpp
  TemperatureCPU_Tests.cpp
  main.cpp
)



ADD_EXECUTABLE(iTemperatureCPU ${SRC})

TARGET_LINK_LIBRARIES(iTemperatureCPU
   MOOS
   mbutil
   m
   pthread)

