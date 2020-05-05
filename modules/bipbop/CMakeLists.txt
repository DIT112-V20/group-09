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

add_library (BipBop INTERFACE)

target_include_directories (BipBop INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/bipbop
        ${CMAKE_SOURCE_DIR}/include)
target_sources (BipBop INTERFACE
        include/bipbop/boip/Address.hxx
        include/bipbop/boip/Endpoint.hxx
        include/bipbop/boip/Parse.hxx
        include/bipbop/boip/impl/Parse.txx
        include/bipbop/boip/Message.hxx
        )
target_compile_definitions (BipBop INTERFACE )
target_link_libraries (BipBop INTERFACE
        stdpolyfills::stdpolyfills
        Microsoft.GSL::GSL
        range-v3::range-v3
        fmt::fmt-header-only
        Boost::asio)

#add_executable (TestsBipBop )
#set_target_properties (TestsBipBop PROPERTIES LINKER_LANGUAGE CXX)
#target_link_libraries (TestsBipBop BipBop Catch2)
#target_compile_definitions (TestsBipBop PUBLIC ${FLAG_ENABLE_DEBUG})

#catch_discover_tests (TestsBipBop)
#add_dependencies(TestTag TestsBipBop)

