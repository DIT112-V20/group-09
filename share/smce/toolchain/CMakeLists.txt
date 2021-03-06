#
#  CMakeLists.txt (runtime)
#  Copyright 2020 ItJustWorksTM
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#

#
# Input variables:
# SOURCE_FILE: Path to the source file
# SOURCE_TYPE:
#       - "sketch" for single .cpp/.ino/.pde
#       - "sketch_dir" for GLOBing .ino/.pde files in parent directory
#       - "sketch_dir_recurse" for GLOB_RECURSEing .ino/.pde files in parent directory (unsupported)
# ARDUINOCLI: Path to ArduinoCLI
#

cmake_minimum_required (VERSION 3.13)
set (CMAKE_POSITION_INDEPENDENT_CODE ON)

set (SILENCE_UNUSED ${ARDUINOCLI_PATH})
message (STATUS "Processing ${SOURCE_FILE}")

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_EXTENSIONS OFF)

add_compile_definitions (SMCE__COMPILING_USERCODE boolean=bool)

include_directories (${CMAKE_SOURCE_DIR}/include/ardpolyfills)
if (WIN32)
    add_compile_definitions (VC_EXTRALEAN WIN32_LEAN_AND_MEAN)
endif ()

get_filename_component (BUILD_DIRNAME ${CMAKE_BINARY_DIR} NAME)
project (${BUILD_DIRNAME} LANGUAGES CXX)
set (HOME_DIRNAME ${CMAKE_SOURCE_DIR})


set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/Modules)
include (Preproc)

fetch_ardcli ("${ARDUINOCLI_PATH}")
install_libs ("${ARDUINOCLI_PATH}")
identify_source (${SOURCE_FILE} ${SOURCE_TYPE})
if (SOURCE_IS_INO)
    ino_preprocess ("${SOURCE_FILE}" "${ARDUINOCLI_PATH}")
endif ()

file (TO_CMAKE_PATH "${SOURCE_FILE}" SOURCE_FILE)
add_library (${BUILD_DIRNAME} MODULE)
target_sources (${BUILD_DIRNAME} PUBLIC "${SOURCE_FILE}")
add_custom_command (
        TARGET ${BUILD_DIRNAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND}
        ARGS -E rename $<TARGET_FILE:${BUILD_DIRNAME}> ${CMAKE_CURRENT_BINARY_DIR}/${BUILD_DIRNAME})

include (ArduinoRuntimeLocation)
add_library (ArdRuntime SHARED IMPORTED)
set_target_properties (ArdRuntime PROPERTIES
        IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/lib/${ARDUINO_RUNTIME_FILENAME}"
        IMPORTED_IMPLIB "${CMAKE_SOURCE_DIR}/lib/${ARDUINO_RUNTIME_LINK_FILENAME}")

target_link_libraries (${BUILD_DIRNAME} PUBLIC ArdRuntime)

set (SMARTCAR_SOURCE_PATH "${CMAKE_SOURCE_DIR}/libraries/smartcar_shield/src")
file (GLOB_RECURSE SMARTCAR_SOURCES ${SMARTCAR_SOURCE_PATH}/*)
message ("SMARTCAR_SOURCE_PATH: ${SMARTCAR_SOURCE_PATH}")
add_library (SmartcarShield STATIC EXCLUDE_FROM_ALL)
target_sources (SmartcarShield PRIVATE ${SMARTCAR_SOURCES})
target_include_directories (SmartcarShield PUBLIC ${SMARTCAR_SOURCE_PATH})
target_link_libraries (SmartcarShield PUBLIC ArdRuntime)

set (VL53L0X_SOURCE_PATH "${CMAKE_SOURCE_DIR}/libraries/VL53L0X")
message ("VL53L0X_SOURCE_PATH: ${VL53L0X_SOURCE_PATH}")
add_library (vl53l0x STATIC EXCLUDE_FROM_ALL)
target_sources (vl53l0x PRIVATE ${VL53L0X_SOURCE_PATH}/VL53L0X.cpp ${VL53L0X_SOURCE_PATH}/VL53L0X.h)
target_include_directories (vl53l0x PUBLIC ${VL53L0X_SOURCE_PATH})
target_link_libraries (vl53l0x PUBLIC ArdRuntime)

target_link_libraries (${BUILD_DIRNAME} PUBLIC SmartcarShield vl53l0x)