#--------------------------------------------------------
# The CMakeLists.txt for:                       pCamera
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  Camera.cpp
  Camera_Info.cpp
  Camera_Tests.cpp
  main.cpp
)



ADD_EXECUTABLE(pCamera ${SRC})

TARGET_LINK_LIBRARIES(pCamera
   MOOS
   mbutil
   m
   pthread)

