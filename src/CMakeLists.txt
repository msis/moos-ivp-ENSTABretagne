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
ADD_SUBDIRECTORY(pCiscrea)
ADD_SUBDIRECTORY(pLocalization)
ADD_SUBDIRECTORY(pControler)
ADD_SUBDIRECTORY(pSonar)
ADD_SUBDIRECTORY(pEchoSonder)
ADD_SUBDIRECTORY(pModem)
ADD_SUBDIRECTORY(pCamera)
ADD_SUBDIRECTORY(pHydrophone)
ADD_SUBDIRECTORY(pTemperatureCPU)
ADD_SUBDIRECTORY(pInertialMeasurementUnit)
ADD_SUBDIRECTORY(pLogs)
ADD_SUBDIRECTORY(pPipeFollowing)

##############################################################################
#                           END of CMakeLists.txt
##############################################################################

