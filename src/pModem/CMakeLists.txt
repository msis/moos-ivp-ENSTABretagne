#--------------------------------------------------------
# The CMakeLists.txt for:                       pModem
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  Modem.cpp
  Modem_Info.cpp
  Modem_Tests.cpp
  Communication.cpp
  main.cpp
)



ADD_EXECUTABLE(pModem ${SRC})

TARGET_LINK_LIBRARIES(pModem
   MOOS
   mbutil
   m
   pthread)

