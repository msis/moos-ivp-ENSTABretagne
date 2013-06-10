#--------------------------------------------------------
# The CMakeLists.txt for:                       pHydrophone
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  Hydrophone.cpp
  Hydrophone_Info.cpp
  Hydrophone_Tests.cpp
  main.cpp
)



ADD_EXECUTABLE(pHydrophone ${SRC})

TARGET_LINK_LIBRARIES(pHydrophone
   MOOS
   mbutil
   m
   pthread)

