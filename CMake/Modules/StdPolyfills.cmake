#
#  StdPolyfills.cmake
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

add_library (stdpolyfills INTERFACE)

set (CMAKE_REQUIRED_FLAGS "-fconcepts")
file (READ "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Check/compiler_needs_fconcepts.cpp" CHECK_SRC)
check_cxx_source_compiles ("${CHECK_SRC}" NEEDS_FCONCEPTS)
unset (CMAKE_REQUIRED_FLAGS)
unset (CHECK_SRC)

if (NEEDS_FCONCEPTS)
    target_compile_options (stdpolyfills INTERFACE "-fconcepts")
endif (NEEDS_FCONCEPTS)

file (READ "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Check/lib_concepts_avail.cpp" CHECK_SRC)
check_cxx_source_compiles ("${CHECK_SRC}" CXX_STDLIB_CONCEPTS_AVAILABLE)
unset (CHECK_SRC)
if (NOT CXX_STDLIB_CONCEPTS_AVAILABLE)
    target_include_directories (stdpolyfills SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/stdpolyfills/polyfill_concepts)
    target_link_libraries (stdpolyfills INTERFACE range-v3::range-v3)
endif (NOT CXX_STDLIB_CONCEPTS_AVAILABLE)

find_package (Threads REQUIRED) # For std::thread
target_link_libraries (stdpolyfills INTERFACE Threads::Threads)

add_library (stdpolyfills::stdpolyfills ALIAS stdpolyfills)

file (READ "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Check/has_libstdcxx.cpp" CHECK_SRC)
check_cxx_source_compiles ("${CHECK_SRC}" HAS_LIBSTDCXX)
file (READ "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Check/has_libcxx.cpp" CHECK_SRC)
check_cxx_source_compiles ("${CHECK_SRC}" HAS_LIBCXX)

if (SMCE_STDLIB_DEBUG)
    if (HAS_LIBCXX AND NOT APPLE)
        add_compile_definitions (_LIBCPP_DEBUG=1)
    elseif (HAS_LIBSTDCXX)
        add_compile_definitions (_GLIBCXX_DEBUG=1)
    endif ()
endif ()

if (MINGW AND MINGW_THREADS_WIN32)
    target_compile_definitions (stdpolyfills INTERFACE -DSMCE_THREADS_MINGW_WIN32=1)
endif ()