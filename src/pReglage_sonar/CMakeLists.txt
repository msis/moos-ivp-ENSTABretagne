#--------------------------------------------------------
# The CMakeLists.txt for:                       pReglage_sonar
# Author(s):                              QJ
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  Reglage_sonar.cpp
  Reglage_sonar_Info.cpp
  main.cpp
  sonar.cpp
  localisation.c
)



ADD_EXECUTABLE(pReglage_sonar ${SRC})

TARGET_LINK_LIBRARIES(pReglage_sonar
   ${MOOS_LIBRARIES}
   mbutil
   m
   pthread)

