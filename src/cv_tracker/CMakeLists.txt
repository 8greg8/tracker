cmake_minimum_required(VERSION 3.11)
project(cv_tracker)
#-------------------------------------------------------------------------------
# Version number
#-------------------------------------------------------------------------------
set(CV_VERSION_MAJOR 2)
set(CV_VERSION_MINOR 0)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)


#-------------------------------------------------------------------------------
# Header files
#-------------------------------------------------------------------------------
# Configure a header file to pass some of the CMake settings
# to the source code
set(CONFIG_HPP "config.hpp")
set(CV_CONFIG ${SOURCE_DIR}/${CONFIG_HPP})
configure_file(${CV_CONFIG}.in
    ${CV_CONFIG}
    )

#-------------------------------------------------------------------------------
# Declare dependency
#-------------------------------------------------------------------------------
find_package(OpenCV 3.1 REQUIRED COMPONENTS core imgproc videoio highgui tracking)
find_package(Boost 1.53 REQUIRED COMPONENTS filesystem)

#-------------------------------------------------------------------------------
# Create targets and set properites
#-------------------------------------------------------------------------------
# Add executable
add_executable(cv_tracker 
    src/main.cpp
    src/config.hpp
)


# Link librares
target_link_libraries(cv_tracker
    ${OpenCV_LIBS}
    ${Boost_LIBRARIES}
    Util::util
    Tracker::tracker
    Terminal::terminal
    Time::time
)
target_compile_features(cv_tracker
    PRIVATE
        cxx_std_11
)
#-------------------------------------------------------------------------------
# Add install targets
#-------------------------------------------------------------------------------
INSTALL(TARGETS cv_tracker 
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)


