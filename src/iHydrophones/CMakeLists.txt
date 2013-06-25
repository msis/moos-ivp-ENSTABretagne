#--------------------------------------------------------
# The CMakeLists.txt for:                       pHydrophones
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  Hydrophones.cpp
  Hydrophones_Info.cpp
  Hydrophones_Tests.cpp
  main.cpp
)



ADD_EXECUTABLE(pHydrophones ${SRC})

TARGET_LINK_LIBRARIES(pHydrophones
   MOOS
   mbutil
   m
   pthread)

