#--------------------------------------------------------
# The CMakeLists.txt for:                       pMove
# Author(s):                              QJ
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  Move.cpp
  IMove.cpp
  Move_Info.cpp
  main.cpp
	state.cpp
	autom.cpp
	sonar.cpp
	localisation.c
)



ADD_EXECUTABLE(pMove ${SRC})

TARGET_LINK_LIBRARIES(pMove
   ${MOOS_LIBRARIES}
   mbutil
   m
   pthread)

