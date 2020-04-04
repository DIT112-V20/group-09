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

cmake_minimum_required (VERSION 3.13)
project (smartcar_emul LANGUAGES CXX C)

set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Modules")

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory (thirdparty/fmt)
add_subdirectory (thirdparty/GSL)
add_subdirectory (thirdparty/magic_enum)
add_subdirectory (thirdparty/nameof)

set (RANGES_CXX_STD 20)
set (RANGES_PREFER_REAL_CONCEPTS ON CACHE BOOL "")
set (RANGES_DEEP_STL_INTEGRATION ON CACHE BOOL "")
add_subdirectory (thirdparty/range-v3)

include (SetupUrho3D)
include (StdPolyfills)

add_library (smartcar_emul_deps INTERFACE)
target_link_libraries (smartcar_emul_deps INTERFACE
        fmt::fmt
        Microsoft.GSL::GSL
        magic_enum::magic_enum
        nameof::nameof
        range-v3::range-v3
        Urho3D::Urho3D
        stdpolyfills::stdpolyfills
        )

add_executable (smartcar_emul src/main.cxx)
target_include_directories (smartcar_emul PRIVATE
        thirdparty/Boost.DLL/include
        include
        )
target_link_libraries (smartcar_emul PRIVATE smartcar_emul_deps)
target_compile_options (smartcar_emul PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/permissive->)

target_sources (smartcar_emul PRIVATE
        )

add_subdirectory (thirdparty/Catch2)
add_executable (smartcar_emul_tests test/sanity.cxx)
target_link_libraries (smartcar_emul_tests PRIVATE
        smartcar_emul_deps
        Catch2)


set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/Catch2/contrib")
include (CTest)
include (Catch)
catch_discover_tests (smartcar_emul_tests)