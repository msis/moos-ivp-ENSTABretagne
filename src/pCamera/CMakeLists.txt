#--------------------------------------------------------
# The CMakeLists.txt for:                       pFrontCamera
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  FrontCamera.cpp
  FrontCamera_Info.cpp
  FrontCamera_Tests.cpp
  main.cpp
)



ADD_EXECUTABLE(pFrontCamera ${SRC})

TARGET_LINK_LIBRARIES(pFrontCamera
   MOOS
   mbutil
   m
   pthread)

