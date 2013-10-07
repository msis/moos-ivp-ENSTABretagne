##############################################################################
# FILE:        moos-ivp-extend/src/CMakeLists.txt
# DATE:        2010/09/07
# DESCRIPTION: CMakeLists.txt file for the moos-ivp-extend source directory
##############################################################################


#============================================================================
# Add the libraries in the current directory to the include path
#============================================================================
FILE(GLOB LOCAL_LIBRARY_DIRS ./lib_* /usr/local/lib)
INCLUDE_DIRECTORIES(${LOCAL_LIBRARY_DIRS} /usr/local/include/opencv)

#============================================================================
# List the subdirectories to build...
#============================================================================
ADD_SUBDIRECTORY(iCamera)
ADD_SUBDIRECTORY(iCiscrea)
ADD_SUBDIRECTORY(iEchoSounder)
ADD_SUBDIRECTORY(iHydrophones)
ADD_SUBDIRECTORY(iInertialMeasurementUnit)
ADD_SUBDIRECTORY(iModem)
ADD_SUBDIRECTORY(iSonar)
ADD_SUBDIRECTORY(pBuoyDetection)
ADD_SUBDIRECTORY(pControler)
ADD_SUBDIRECTORY(pLocalization)
ADD_SUBDIRECTORY(pLogs)
ADD_SUBDIRECTORY(pPingLocator)
ADD_SUBDIRECTORY(pPipeFollowing)
ADD_SUBDIRECTORY(pRegulator)
ADD_SUBDIRECTORY(iTemperatureCPU)
ADD_SUBDIRECTORY(pWallFollowing)
ADD_SUBDIRECTORY(uJoystick)
ADD_SUBDIRECTORY(pSensorViewer)
ADD_SUBDIRECTORY(pLateralRegulator)
ADD_SUBDIRECTORY(pValidationGate)
ADD_SUBDIRECTORY(pAroundTheIsland)
ADD_SUBDIRECTORY(pSonarThdViewer)
ADD_SUBDIRECTORY(pWayPoints)
ADD_SUBDIRECTORY(pMove)
ADD_SUBDIRECTORY(pLoc)
ADD_SUBDIRECTORY(pSimulateur)
ADD_SUBDIRECTORY(pReglage_sonar)


##############################################################################
#                           END of CMakeLists.txt
##############################################################################

