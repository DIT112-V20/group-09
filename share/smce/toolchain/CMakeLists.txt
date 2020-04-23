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

set (SILENCE_UNUSED ${ARDUINOCLI_PATH})
message (STATUS "Processing ${SOURCE_FILE}")

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_EXTENSIONS OFF)

if (WIN32)
    add_compile_definitions (VC_EXTRALEAN WIN32_LEAN_AND_MEAN)
endif ()

get_filename_component (BUILD_DIRNAME ${CMAKE_BINARY_DIR} NAME)
project (${BUILD_DIRNAME} LANGUAGES CXX)
set (HOME_DIRNAME ${CMAKE_SOURCE_DIR})


set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/Modules)
include (Preproc)

fetch_ardcli ("${ARDUINOCLI_PATH}")
identify_source (${SOURCE_FILE} ${SOURCE_TYPE})
if (SOURCE_IS_INO)
    ino_preprocess ("${SOURCE_FILE}" "${ARDUINOCLI_PATH}")
endif ()

add_library (${BUILD_DIRNAME} MODULE)
target_sources (${BUILD_DIRNAME} PRIVATE "${SOURCE_FILE}")
add_custom_command (
		TARGET ${BUILD_DIRNAME}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND}
 		ARGS -E rename $<TARGET_FILE:${BUILD_DIRNAME}> ${CMAKE_CURRENT_BINARY_DIR}/${BUILD_DIRNAME})

if (SOURCE_IS_INO)
    target_sources (${BUILD_DIRNAME} PRIVATE "${CMAKE_SOURCE_DIR}/Fragments/symbols.cxx")
endif ()

include (ArduinoRuntimeLocation)
add_library (ArdRuntime STATIC IMPORTED)
set_target_properties (ArdRuntime PROPERTIES
        IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/lib/${ARDUINO_RUNTIME_FILENAME}")
target_include_directories (ArdRuntime SYSTEM INTERFACE ${CMAKE_SOURCE_DIR}/include/ardpolyfills)

set (CMAKE_POSITION_INDEPENDENT_CODE ON)

target_link_libraries (${BUILD_DIRNAME} PRIVATE ArdRuntime)