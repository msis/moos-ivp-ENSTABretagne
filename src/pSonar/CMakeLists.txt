#--------------------------------------------------------
# The CMakeLists.txt for:                       pSonar
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  Sonar.cpp
  Sonar_Info.cpp
  Sonar_Tests.cpp
  main.cpp
  SonarDF.cpp
)

SET(MINIKING_SRC
  MiniKing/MiniKingCfg.c
  MiniKing/MiniKingCore.c
  MiniKing/MiniKingMgr.c
)

SET(OSUTILS_SRC
  OSUtils/OSCore.c
  OSUtils/OSCriticalSection.c
  OSUtils/OSEv.c
  OSUtils/OSIPEv.c
  OSUtils/OSIPMutex.c
  OSUtils/OSIPSem.c
  OSUtils/OSMisc.c
  OSUtils/OSRS232Port.c
  OSUtils/OSSem.c
  OSUtils/OSThread.c
  OSUtils/OSTime.c
  OSUtils/OSTimer.c
)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_DEBUG")

ADD_EXECUTABLE(pSonar ${SRC} ${MINIKING_SRC} ${OSUTILS_SRC})

include_directories(MiniKing/ OSUtils/)

TARGET_LINK_LIBRARIES(pSonar
   MOOS
   mbutil
   m
   pthread
   rt)

