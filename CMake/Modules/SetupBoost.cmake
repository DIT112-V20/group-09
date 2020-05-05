#
#  SetupBoost.cmake
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

include (GetBoost)

add_library (Boost_dll INTERFACE)
target_compile_definitions (Boost_dll INTERFACE BOOST_DLL_USE_STD_FS=1)
target_link_libraries (Boost_dll INTERFACE Boost::headers)

if (WIN32)
    target_link_libraries (Boost_dll INTERFACE Kernel32)
else ()
    target_link_libraries (Boost_dll INTERFACE dl)
endif ()

add_library (Boost::dll ALIAS Boost_dll)



add_library (Boost_asio INTERFACE)
target_compile_definitions (Boost_asio INTERFACE BOOST_DLL_USE_STD_FS=1)
target_link_libraries (Boost_asio INTERFACE Boost::headers Boost::system)
add_library (Boost::asio ALIAS Boost_asio)



add_library (Boost_staticstring INTERFACE)
target_compile_definitions (Boost_staticstring INTERFACE BOOST_STATIC_STRING_STANDALONE=1)
add_library (Boost::static_string ALIAS Boost_staticstring)
