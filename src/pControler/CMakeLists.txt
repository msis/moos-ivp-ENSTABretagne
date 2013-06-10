#--------------------------------------------------------
# The CMakeLists.txt for:                       pControler
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  Controler.cpp
  Controler_Info.cpp
  Controler_Tests.cpp
  main.cpp
)



ADD_EXECUTABLE(pControler ${SRC})

TARGET_LINK_LIBRARIES(pControler
   MOOS
   mbutil
   m
   pthread)

