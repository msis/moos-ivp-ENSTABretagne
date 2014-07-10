##############################################################################
# FILE:        moos-ivp-extend/src/CMakeLists.txt
# DATE:        2010/09/07
# DESCRIPTION: CMakeLists.txt file for the moos-ivp-extend source directory
##############################################################################


#============================================================================
# Add the libraries in the current directory to the include path
#============================================================================
FILE(GLOB LOCAL_LIBRARY_DIRS ./lib_* /usr/local/lib)
INCLUDE_DIRECTORIES(${LOCAL_LIBRARY_DIRS})

#============================================================================
# List the subdirectories to build...
#============================================================================

# sensors/actuators interfaces
ADD_SUBDIRECTORY(iCamera)
ADD_SUBDIRECTORY(iCiscrea)
ADD_SUBDIRECTORY(iEchoSounder)
ADD_SUBDIRECTORY(iHydrophones)
ADD_SUBDIRECTORY(iInertialMeasurementUnit)
ADD_SUBDIRECTORY(iModem)
ADD_SUBDIRECTORY(iSonar)
ADD_SUBDIRECTORY(iTemperatureCPU)
ADD_SUBDIRECTORY(iGPSoE)

# 
ADD_SUBDIRECTORY(pBuoyDetection)
ADD_SUBDIRECTORY(pControler)
ADD_SUBDIRECTORY(pLocalization)
ADD_SUBDIRECTORY(pLogs)
ADD_SUBDIRECTORY(pPingLocator)
ADD_SUBDIRECTORY(pPipeFollowing)
ADD_SUBDIRECTORY(pRegulator)


ADD_SUBDIRECTORY(uJoystick)

ADD_SUBDIRECTORY(pLateralRegulator)
ADD_SUBDIRECTORY(pValidationGate)
ADD_SUBDIRECTORY(pWayPoints)
ADD_SUBDIRECTORY(pMove)
ADD_SUBDIRECTORY(pLoc)
ADD_SUBDIRECTORY(pSimulateur)
ADD_SUBDIRECTORY(pReglage_sonar)
ADD_SUBDIRECTORY(pVideoCodec)

# autonomy
ADD_SUBDIRECTORY(pAroundTheIsland)
ADD_SUBDIRECTORY(pWallFollowing)

# visualisation
ADD_SUBDIRECTORY(pSensorViewer)
ADD_SUBDIRECTORY(pWayPointsViewer)
ADD_SUBDIRECTORY(pSonarThdViewer)

##############################################################################
#                           END of CMakeLists.txt
##############################################################################

