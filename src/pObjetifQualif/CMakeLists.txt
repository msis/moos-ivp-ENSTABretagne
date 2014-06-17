#--------------------------------------------------------
# The CMakeLists.txt for:                       pObjetifQualif
# Author(s):                              MSIS
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  ObjetifQualif.cpp
  ObjetifQualif_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(pObjetifQualif ${SRC})

TARGET_LINK_LIBRARIES(pObjetifQualif
   ${MOOS_LIBRARIES}
   mbutil
   m
   pthread)

