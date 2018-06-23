cmake_minimum_required(VERSION 3.11)
project(custom_tracker)
#-------------------------------------------------------------------------------
# Version number
#-------------------------------------------------------------------------------
set(CUSTOM_VERSION_MAJOR 2)
set(CUSTOM_VERSION_MINOR 0)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)


#-------------------------------------------------------------------------------
# Header files
#-------------------------------------------------------------------------------
# Configure a header file to pass some of the CMake settings
# to the source code
set(CONFIG_HPP "config.hpp")
set(CONFIG ${SOURCE_DIR}/${CONFIG_HPP})
configure_file(${CONFIG}.in
    ${CONFIG}
)

#-------------------------------------------------------------------------------
# Declare dependency
#-------------------------------------------------------------------------------
find_package(OpenCV 3.1 REQUIRED COMPONENTS core imgproc videoio highgui tracking)
find_package(dlib REQUIRED)

# Add executable
add_executable(custom_tracker 
    src/main.cpp
    src/config.hpp
)
add_dependencies(custom_tracker
    opentld
    ccv
)

target_include_directories(custom_tracker
    PRIVATE
    ${OpenTLD_INCLUDE_DIRS}
)
# Link librares
target_link_libraries(custom_tracker
    ${DLIB_LIBS}
    ${OpenCV_LIBS}
    Util::util
    Tracker::tracker
    Terminal::terminal
    Time::time
    ccv
    opentld
)
target_compile_features(cv_tracker
    PUBLIC
        cxx_std_11
)

#-------------------------------------------------------------------------------
# Add install targets
#-------------------------------------------------------------------------------
INSTALL(TARGETS custom_tracker 
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)


