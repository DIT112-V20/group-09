#
#  BoostDLL.cmake
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

add_library (Boost_dll INTERFACE)

target_compile_definitions (Boost_dll INTERFACE BOOST_DLL_USE_STD_FS=1)
target_include_directories (Boost_dll INTERFACE
        thirdparty/Boost.DLL/include
        thirdparty/Boost.Core/include)
target_link_libraries (Boost_dll INTERFACE
        Boost::config
        Boost::core
        Boost::move
        Boost::throw_exception
        Boost::type_traits
        Boost::smart_ptr
        Boost::winapi)

if (WIN32)
    target_link_libraries (Boost_dll INTERFACE Kernel32)
else ()
    target_link_libraries (Boost_dll INTERFACE dl)
endif ()

add_library(Boost::dll ALIAS Boost_dll)