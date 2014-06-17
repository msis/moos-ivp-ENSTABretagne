#--------------------------------------------------------
# The CMakeLists.txt for:                       pLoc
# Author(s):                              QJ
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  Loc.cpp
  Loc_Info.cpp
  localisation.c
  sonar.cpp
  main.cpp
)



ADD_EXECUTABLE(pLoc ${SRC})

TARGET_LINK_LIBRARIES(pLoc
   ${MOOS_LIBRARIES}
   mbutil
   m
   pthread)

