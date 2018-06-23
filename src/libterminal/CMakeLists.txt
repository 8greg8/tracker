cmake_minimum_required(VERSION 3.11)
project(libterminal VERSION 1.0.0 LANGUAGES CXX)
 
################################################################################
# Declare dependency
find_package(OpenCV 3.1 REQUIRED COMPONENTS core highgui)
find_package(Boost 1.53 REQUIRED)

################################################################################
# Create targets and set properties
add_library(terminal 
    src/terminalparser.cpp
    src/terminalparser2.cpp
)
# Add an alias so that the library can be used inside build tree
add_library(Terminal::terminal ALIAS terminal)

add_dependencies(terminal Tracker::tracker Util::util)

target_include_directories(terminal
    PUBLIC # used by others and used internally in the project
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set_property(TARGET terminal PROPERTY
    POSITION INDEPENDENT_CODE ON
)
target_compile_features(terminal
    PRIVATE
        cxx_std_11
)
target_link_libraries(terminal
    ${OpenCV_LIBS}
    ${Boost_LIBRARIES}
    Tracker::tracker
    Util::util
)

################################################################################
# Instalation instructions
include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/Terminal)

install(TARGETS terminal
    EXPORT terminal-targets
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" # for static lib    
    )

set_target_properties(terminal PROPERTIES 
        EXPORT_NAME Terminal #This is required so that exported target has the right name
        VERSION "${PROJECT_VERSION}"
        SOVERSION "${PROJECT_VERSION_MAJOR}"
)

# Install public headers 
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# This makes project importable from the install directory
# Export the targets to a script
install(EXPORT terminal-targets 
    FILE 
        TerminalTargets.cmake
    NAMESPACE 
        Terminal::
    DESTINATION 
        ${INSTALL_CONFIGDIR}
)

# Create ConfigVersion.cmake
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/TerminalConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)
configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/TerminalConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/TerminalConfig.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)


# Install config
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/TerminalConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/TerminalConfigVersion.cmake
    DESTINATION ${INSTALL_CONFIGDIR}
)

################################################################################
# Exporting from the build tree
# This makes project importable from the build directory
export(EXPORT terminal-targets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/TerminalTargets.cmake
    NAMESPACE Terminal::
)

# Register package in users's package registry
export(PACKAGE Terminal)
