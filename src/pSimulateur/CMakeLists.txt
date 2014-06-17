#--------------------------------------------------------
# The CMakeLists.txt for:                       pSimulateur
# Author(s):                              QJ
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  Simulateur.cpp
  Simulateur_Info.cpp 
  main.cpp
)


ADD_EXECUTABLE(pSimulateur ${SRC})

TARGET_LINK_LIBRARIES(pSimulateur
   ${MOOS_LIBRARIES}
   mbutil
   m
   pthread)

