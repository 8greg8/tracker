cmake_minimum_required(VERSION 3.11)
project(libtracker VERSION 1.0.0 LANGUAGES CXX)
 
################################################################################
# Declare dependency
find_package(OpenCV 3.1 REQUIRED COMPONENTS tracking)

################################################################################
# Create targets and set properties
add_library(tracker 
    src/tracker.cpp
    src/trackerfile.cpp
)
# Add an alias so that the library can be used inside build tree
add_library(Tracker::tracker ALIAS tracker)


add_dependencies(tracker Util::util)

target_include_directories(tracker
    PUBLIC # used by others and used internally in the project
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set_property(TARGET tracker PROPERTY
    POSITION INDEPENDENT_CODE ON
)
target_compile_features(tracker
    PRIVATE
        cxx_std_11
)

target_link_libraries(tracker
    ${OpenCV_LIBS}
    Util::util
)

################################################################################
# Instalation instructions
include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/Tracker)

install(TARGETS tracker
    EXPORT tracker-targets
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" # for static lib    
    )

set_target_properties(tracker PROPERTIES 
        EXPORT_NAME Tracker #This is required so that exported target has the right name
        VERSION "${PROJECT_VERSION}"
        SOVERSION "${PROJECT_VERSION_MAJOR}"
)

# Install public headers 
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# This makes project importable from the install directory
# Export the targets to a script
install(EXPORT tracker-targets 
    FILE 
        TrackerTargets.cmake
    NAMESPACE 
        Tracker::
    DESTINATION 
        ${INSTALL_CONFIGDIR}
)

# Create ConfigVersion.cmake
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/TrackerConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)
configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/TrackerConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/TrackerConfig.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)


# Install config
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/TrackerConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/TrackerConfigVersion.cmake
    DESTINATION ${INSTALL_CONFIGDIR}
)

################################################################################
# Exporting from the build tree
# This makes project importable from the build directory
export(EXPORT tracker-targets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/TrackerTargets.cmake
    NAMESPACE Tracker::
)

# Register package in users's package registry
export(PACKAGE Tracker)
