cmake_minimum_required(VERSION 3.11)

project(tracker)

#-------------------------------------------------------------------------------
# Versions
#-------------------------------------------------------------------------------
SET(PROJECT_VERSION_MAJOR 1)
SET(PROJECT_VERSION_MINOR 0)
SET(PROJECT_VERSION_PATCH 0)
SET(PROJECT_VERSION 
    "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}"
)


#-------------------------------------------------------------------------------
# Build version
#-------------------------------------------------------------------------------
IF( NOT CMAKE_BUILD_TYPE )
    SET(CMAKE_BUILD_TYPE Release)
    MESSAGE("Release build.")
ELSEIF( CMAKE_BUILD_TYPE STREQUAL "Debug")
    ADD_DEFINITIONS(-DDEBUG)
    MESSAGE("Debug build")
ENDIF()


#-------------------------------------------------------------------------------
# Options
#-------------------------------------------------------------------------------
OPTION(BUILD_SHARED_LIBS "Build shared librares" ON)
set(CMAKE_CXX_STANDARD 11)


#-------------------------------------------------------------------------------
# Include cmake functions
#-------------------------------------------------------------------------------
include(ExternalProject)
include(ProcessorCount)
include(GNUInstallDirs)

#-------------------------------------------------------------------------------
# Count processes
#-------------------------------------------------------------------------------
ProcessorCount(NPROC)
if(NOT NPROC)
    SET(NPROC 4)
endif()

#-------------------------------------------------------------------------------
# Include 3rd party
#-------------------------------------------------------------------------------

include(3rdparty/opentld.cmake)
include(3rdparty/ccv.cmake)



#-------------------------------------------------------------------------------
# Add subdirectories to get libraries
#-------------------------------------------------------------------------------
add_subdirectory(libcore)
add_subdirectory(libutil)
add_subdirectory(libtracker)
add_subdirectory(libterminal)
add_subdirectory(libtime)


#-------------------------------------------------------------------------------
# Add subdirectories to get programs
#-------------------------------------------------------------------------------
add_subdirectory(cv_tracker)
add_subdirectory(custom_tracker)
