cmake_minimum_required(VERSION 3.11)
project(libtime VERSION 1.0.0 LANGUAGES CXX)

################################################################################
# Declare dependency
find_package(OpenCV 3.1 REQUIRED COMPONENTS videoio)

################################################################################
# Create targets and set properties
add_library(time 
    src/basetimer.cpp
    src/videotimer.cpp
)
# Add an alias so that the library can be used inside build tree
add_library(Time::time ALIAS time)

target_include_directories(time
    PUBLIC # used by others and used internally in the project
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(time
    PRIVATE
        cxx_std_11
)

target_link_libraries(time
    ${OpenCV_LIBS}
)

################################################################################
# Instalation instructions
include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/Time)

install(TARGETS time
    EXPORT time-targets
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" # for static lib    
    )

set_target_properties(time PROPERTIES 
        EXPORT_NAME Time #This is required so that exported target has the right name
        VERSION "${PROJECT_VERSION}"
        SOVERSION "${PROJECT_VERSION_MAJOR}"
)

# Install public headers 
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# This makes project importable from the install directory
# Export the targets to a script
install(EXPORT time-targets 
    FILE 
        TimeTargets.cmake
    NAMESPACE 
        Time::
    DESTINATION 
        ${INSTALL_CONFIGDIR}
)

# Create ConfigVersion.cmake
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/TimeConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)
configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/TimeConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/TimeConfig.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)


# Install config
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/TimeConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/TimeConfigVersion.cmake
    DESTINATION ${INSTALL_CONFIGDIR}
)

################################################################################
# Exporting from the build tree
# This makes project importable from the build directory
export(EXPORT time-targets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/TimeTargets.cmake
    NAMESPACE Time::
)

# Register package in users's package registry
export(PACKAGE Time)
