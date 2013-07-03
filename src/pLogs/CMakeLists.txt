#--------------------------------------------------------
# The CMakeLists.txt for:                       pLogs
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  Logs.cpp
  Logs_Info.cpp
  Logs_Tests.cpp
  main.cpp
)



ADD_EXECUTABLE(pLogs ${SRC})

TARGET_LINK_LIBRARIES(pLogs
   MOOS
   mbutil
   m
   pthread
   opencv_highgui
   opencv_core)

