#
#  CMakeLists.txt
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

add_library (Ardrivo INTERFACE)

target_include_directories (Ardrivo INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ardrivo
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/ardpolyfills)
target_sources (Ardrivo INTERFACE
        include/ardrivo/BoardDataDef.hxx
        include/ardrivo/Entrypoint.hxx
        include/ardrivo/Error.hxx
        src/Arduino.cxx
        src/Entrypoint.cxx
        src/HardwareSerial.cxx
        src/ProtectedAlloc.cxx
        src/SPI.cxx
        src/Servo.cxx
        src/Stream.cxx
        src/String.cxx
        src/Wire.cxx)
target_compile_definitions (Ardrivo INTERFACE SMCE__COMPILING_ARDUINO_CXX=1)
target_link_libraries (Ardrivo INTERFACE
        stdpolyfills::stdpolyfills
        range-v3::range-v3
        fmt::fmt-header-only
        Microsoft.GSL::GSL)

add_executable (TestsArdrivo test/CatchMain.cxx test/Servo.cxx test/SPI.cxx test/Wire.cxx)
set_target_properties (TestsArdrivo PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries (TestsArdrivo Ardrivo Catch2)
target_compile_definitions (Ardrivo INTERFACE
        SMCE__COMPILING_ARDUINO_CXX=1) # Avoid Windows header leak

catch_discover_tests (TestsArdrivo)
add_dependencies(TestTag TestsArdrivo)

