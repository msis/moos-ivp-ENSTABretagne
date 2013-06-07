##############################################################################
# FILE:        moos-ivp-ENSTABretagne/src/CMakeLists.txt
# DATE:        2013/06/07
# DESCRIPTION: CMakeLists.txt file for the moos-ivp-ENSTABretagne source directory
##############################################################################


#============================================================================
# Add the libraries in the current directory to the include path
#============================================================================
FILE(GLOB LOCAL_LIBRARY_DIRS ./lib_* ../lib)
INCLUDE_DIRECTORIES(${LOCAL_LIBRARY_DIRS})
LINK_DIRECTORIES(${LOCAL_LIBRARY_DIRS})

#============================================================================
# List the subdirectories to build...
#============================================================================

#ADD_SUBDIRECTORY(name_of_directory)

##############################################################################
#                           END of CMakeLists.txt
##############################################################################

