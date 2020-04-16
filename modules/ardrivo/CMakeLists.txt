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
        src/entrypoint.cxx)
target_compile_definitions (Ardrivo INTERFACE)
target_link_libraries (Ardrivo INTERFACE stdpolyfills::stdpolyfills)

#add_executable (TestsArdrivo)
#set_target_properties (TestsArdrivo PROPERTIES LINKER_LANGUAGE CXX)
#target_link_libraries (TestsArdrivo Ardrivo Catch2)

#catch_discover_tests (TestsArdrivo)
#add_dependencies(TestTag TestsArdrivo)

