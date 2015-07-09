#
# SMI Eye Tracker plugin for OSVR
#
cmake_minimum_required(VERSION 2.8.12)
project(com_osvr_SMI_EyeTracker)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# This looks for an osvrConfig.cmake file - most of the time it can be
# autodetected but you might need to create/extend CMAKE_PREFIX_PATH to include something like
# C:/Users/Ryan/Desktop/build/OSVR-Core-vc12 or
# C:/Users/Ryan/Downloads/OSVR-Core-Snapshot-v0.1-406-gaa55515-build54-vs12-32bit
# in the CMake GUI or command line.
find_package(osvr REQUIRED)
find_package(SMI REQUIRED)

# Generate our JSON header file
osvr_convert_json(com_osvr_SMI_EyeTracker_json
    com_osvr_SMI_EyeTracker.json
    "${CMAKE_CURRENT_BINARY_DIR}/com_osvr_SMI_EyeTracker_json.h")

# Be able to find our generated header file.
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# Build and install the plugin
osvr_add_plugin(com_osvr_SMI_EyeTracker
    com_osvr_SMI_EyeTracker.cpp
    SMITrackerDevice.h
    "${CMAKE_CURRENT_BINARY_DIR}/com_osvr_SMI_EyeTracker_json.h")

# Link against PluginKit
target_link_libraries(com_osvr_SMI_EyeTracker
    osvr::osvrPluginKitCpp
    SMI::HMDAPI)

# Install docs, license, sample config
install(FILES
    osvr_server_config.SMIeyeTracker.sample.json
    README.md
    LICENSE
    DESTINATION .)