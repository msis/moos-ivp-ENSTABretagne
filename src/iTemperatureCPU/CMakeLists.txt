#--------------------------------------------------------
# The CMakeLists.txt for:                       pTemperatureCPU
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  TemperatureCPU.cpp
  TemperatureCPU_Info.cpp
  TemperatureCPU_Tests.cpp
  main.cpp
)



ADD_EXECUTABLE(pTemperatureCPU ${SRC})

TARGET_LINK_LIBRARIES(pTemperatureCPU
   MOOS
   mbutil
   m
   pthread)

