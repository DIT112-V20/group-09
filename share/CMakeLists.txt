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

cmake_minimum_required (VERSION 3.13)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_EXTENSIONS OFF)

get_filename_component (BUILD_DIRNAME ${CMAKE_BINARY_DIR} NAME)
project (${BUILD_DIRNAME} LANGUAGES CXX)
set (HOME_DIRNAME ${CMAKE_SOURCE_DIR})


# FIXME: add sketch handling
add_library (${BUILD_DIRNAME} MODULE)
target_sources (${BUILD_DIRNAME} PRIVATE "${SOURCE_FILE}")
add_custom_command (
		TARGET ${BUILD_DIRNAME}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND}
 		ARGS -E rename $<TARGET_FILE:${BUILD_DIRNAME}> ${CMAKE_CURRENT_BINARY_DIR}/${BUILD_DIRNAME})

#add_library (ArdRuntime STATIC IMPORTED)
#set_target_properties (ArdRuntime PROPERTIES
#        IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/lib/ArduinoRuntime")
#target_include_directories (ArdRuntime SYSTEM INTERFACE ${CMAKE_SOURCE_DIR}/include/ardpolyfills)

set (CMAKE_POSITION_INDEPENDENT_CODE ON)
#set (SMARTCAR_SOURCE_PATH "${CMAKE_SOURCE_DIR}/libraries/smartcar_shield/src")
#file (GLOB_RECURSE SMARTCAR_SOURCES ${SMARTCAR_SOURCE_PATH}/*)
#add_library (SmartcarShield STATIC ${SMARTCAR_SOURCES})
#target_include_directories (SmartcarShield PUBLIC ${SMARTCAR_SOURCE_PATH})
#target_link_libraries (SmartcarShield PUBLIC ArdRuntime)

#target_link_libraries (${BUILD_DIRNAME} PRIVATE SmartcarShield ArdRuntime)