#--------------------------------------------------------
# The CMakeLists.txt for:                       pPingLocator
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  PingLocator.cpp
  PingLocator_Info.cpp
  PingLocator_Tests.cpp
  main.cpp
)



ADD_EXECUTABLE(pPingLocator ${SRC})

TARGET_LINK_LIBRARIES(pPingLocator
   MOOS
   mbutil
   m
   pthread)

