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

add_library (TryCompile INTERFACE)

target_include_directories (TryCompile INTERFACE
        ${CMAKE_SOURCE_DIR}/thirdparty/Boost.DLL/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/trycompile)
target_sources (TryCompile INTERFACE
        include/trycompile/Source.hxx
        include/trycompile/Object.hxx
        include/trycompile/impl/Object.txx
        include/trycompile/Runtime.hxx
        src/Runtime.cxx
        include/trycompile/Toolchain.hxx
        src/Object.cxx)
target_compile_definitions (TryCompile INTERFACE BOOST_DLL_USE_STD_FS=1)
target_link_libraries (TryCompile INTERFACE stdpolyfills::stdpolyfills)

add_executable (TestsTryCompile)
set_target_properties (TestsTryCompile PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries (TestsTryCompile TryCompile)

include (CTest)
include (Catch)
catch_discover_tests (TestsTryCompile)