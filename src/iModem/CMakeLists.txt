#--------------------------------------------------------
# The CMakeLists.txt for:                       iModem
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  Communication.cpp
  ConversionsBinaireASCII.cpp
  Modem.cpp
  Modem_Info.cpp
  Modem_Tests.cpp
  main.cpp
)



ADD_EXECUTABLE(iModem ${SRC})

TARGET_LINK_LIBRARIES(iModem
   MOOS
   mbutil
   m
   pthread)

