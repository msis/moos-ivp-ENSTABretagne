#--------------------------------------------------------
# The CMakeLists.txt for:                       iSonar
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

SET(SRC
  seanetmsg.cpp
  Sonar.cpp
  Sonar_Info.cpp
  Sonar_Tests.cpp
  main.cpp
)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_DEBUG")

ADD_EXECUTABLE(iSonar ${SRC})

TARGET_LINK_LIBRARIES(iSonar
   MOOS
   mbutil
   m
   pthread
   rt
   )

