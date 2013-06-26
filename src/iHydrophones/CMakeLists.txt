#--------------------------------------------------------
# The CMakeLists.txt for:                       iHydrophones
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  Hydrophones.cpp
  Hydrophones_Info.cpp
  Hydrophones_Tests.cpp
  main.cpp
)



ADD_EXECUTABLE(iHydrophones ${SRC})

TARGET_LINK_LIBRARIES(iHydrophones
   MOOS
   mbutil
   m
   pthread)

