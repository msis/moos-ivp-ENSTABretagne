#--------------------------------------------------------
# The CMakeLists.txt for:                       pLocalization
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  Localization.cpp
  Localization_Info.cpp
  Localization_Tests.cpp
  main.cpp
)



ADD_EXECUTABLE(pLocalization ${SRC})

TARGET_LINK_LIBRARIES(pLocalization
   MOOS
   mbutil
   m
   pthread)

