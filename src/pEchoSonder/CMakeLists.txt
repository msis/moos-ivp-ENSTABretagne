#--------------------------------------------------------
# The CMakeLists.txt for:                       pEchoSonder
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  EchoSonder.cpp
  EchoSonder_Info.cpp
  EchoSonder_Tests.cpp
  main.cpp
)



ADD_EXECUTABLE(pEchoSonder ${SRC})

TARGET_LINK_LIBRARIES(pEchoSonder
   MOOS
   mbutil
   m
   pthread)

