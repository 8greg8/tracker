cmake_minimum_required(VERSION 3.11)
project(libcore VERSION 1.0.0 LANGUAGES CXX)

################################################################################
# Create targets and set properties
add_library(core 
    src/exception.cpp
)

# Add an alias so that the library can be used inside build tree
add_library(Core::core ALIAS core)

target_include_directories(core
    PUBLIC # used by others and used internally in the project
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(core
    PRIVATE
        cxx_std_11
)

################################################################################
# Instalation instructions
include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/Core)

install(TARGETS core
    EXPORT core-targets
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" # for static lib    
    )

set_target_properties(core PROPERTIES 
        EXPORT_NAME Core #This is required so that exported target has the right name
        VERSION "${PROJECT_VERSION}"
        SOVERSION "${PROJECT_VERSION_MAJOR}"
)

# Install public headers 
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# This makes project importable from the install directory
# Export the targets to a script
install(EXPORT core-targets 
    FILE 
        CoreTargets.cmake
    NAMESPACE 
        Core::
    DESTINATION 
        ${INSTALL_CONFIGDIR}
)

# Create ConfigVersion.cmake
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/CoreConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)
configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/CoreConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/CoreConfig.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)


# Install config
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/CoreConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/CoreConfigVersion.cmake
    DESTINATION ${INSTALL_CONFIGDIR}
)

################################################################################
# Exporting from the build tree
# This makes project importable from the build directory
export(EXPORT core-targets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/CoreTargets.cmake
    NAMESPACE Core::
)

# Register package in users's package registry
export(PACKAGE Core)
