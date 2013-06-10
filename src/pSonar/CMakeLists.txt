#--------------------------------------------------------
# The CMakeLists.txt for:                       pSonar
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  Sonar.cpp
  Sonar_Info.cpp
  Sonar_Tests.cpp
  main.cpp
)



ADD_EXECUTABLE(pSonar ${SRC})

TARGET_LINK_LIBRARIES(pSonar
   MOOS
   mbutil
   m
   pthread)

