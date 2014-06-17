#--------------------------------------------------------
# The CMakeLists.txt for:                       pValidationGate
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  ValidationGate.cpp
  ValidationGate_Info.cpp
  ValidationGate_Tests.cpp
  main.cpp
)



ADD_EXECUTABLE(pValidationGate ${SRC})

TARGET_LINK_LIBRARIES(pValidationGate
   MOOS
   mbutil
   m
   pthread)

