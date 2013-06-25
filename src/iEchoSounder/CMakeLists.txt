#--------------------------------------------------------
# The CMakeLists.txt for:                       pEchoSounder
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  EchoSounder.cpp
  EchoSounder_Info.cpp
  EchoSounder_Tests.cpp
  main.cpp
)



ADD_EXECUTABLE(pEchoSounder ${SRC})

TARGET_LINK_LIBRARIES(pEchoSounder
   MOOS
   mbutil
   m
   pthread)

