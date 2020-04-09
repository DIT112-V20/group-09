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

add_library(ConfigLoader INTERFACE)

target_include_directories(ConfigLoader INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/configloader
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/thirdparty/inih)

target_sources(ConfigLoader INTERFACE
        include/configloader/Loader.hxx
        src/Loader.cxx)

target_link_libraries(TryCompile INTERFACE stdpolyfills::stdpolyfills)

add_executable(TestsConfigLoader test/load.cxx)

set_target_properties(TestsConfigLoader PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(TestsConfigLoader ConfigLoader Catch2)

catch_discover_tests(TestsConfigLoader)
add_dependencies(TestTag TestsConfigLoader)
