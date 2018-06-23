cmake_minimum_required(VERSION 3.11)
project(libutil VERSION 1.0.0 LANGUAGES CXX)
 
################################################################################
# Declare dependency
find_package(OpenCV 3.1 REQUIRED COMPONENTS core highgui imgproc)
find_package(Boost REQUIRED COMPONENTS filesystem)
find_package(dlib REQUIRED)

################################################################################
# Create targets and set properties
add_library(util 
    src/ccvconverter.cpp
    src/dlibconverter.cpp
    src/roi.cpp
    src/roiselector.cpp
    src/scaler.cpp
    src/userinteraction.cpp
)
# Add an alias so that the library can be used inside build tree
add_library(Util::util ALIAS util)

add_dependencies(util ccv Core::core)

target_include_directories(util
    PUBLIC # used by others and used internally in the project
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set_property(TARGET util PROPERTY
    POSITION INDEPENDENT_CODE ON
)
target_compile_features(util
    PRIVATE
        cxx_std_11
)
target_link_libraries(util
    ${OpenCV_LIBS}
    ${Boost_LIBRARIES}
    ${dlib_LIBRARIES}
    Core::core
    ccv
)

################################################################################
# Instalation instructions
include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/Util)

install(TARGETS util
    EXPORT util-targets
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" # for static lib    
    )

set_target_properties(util PROPERTIES 
        EXPORT_NAME Util #This is required so that exported target has the right name
        VERSION "${PROJECT_VERSION}"
        SOVERSION "${PROJECT_VERSION_MAJOR}"
)

# Install public headers 
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# This makes project importable from the install directory
# Export the targets to a script
install(EXPORT util-targets 
    FILE 
        UtilTargets.cmake
    NAMESPACE 
        Util::
    DESTINATION 
        ${INSTALL_CONFIGDIR}
)

# Create ConfigVersion.cmake
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/UtilConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)
configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/UtilConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/UtilConfig.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)


# Install config
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/UtilConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/UtilConfigVersion.cmake
    DESTINATION ${INSTALL_CONFIGDIR}
)

################################################################################
# Exporting from the build tree
# This makes project importable from the build directory
export(EXPORT util-targets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/UtilTargets.cmake
    NAMESPACE Util::
)

# Register package in users's package registry
export(PACKAGE Util)
