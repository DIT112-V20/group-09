#
#  GetBoost.cmake
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
# This is derivative work; original source can be found here: https://gitlab.kitware.com/cmake/cmake/issues/18831

set (BOOST_LIBS system thread)


find_package (Boost 1.70 QUIET COMPONENTS ${BOOST_LIBS})

if (NOT Boost_FOUND)

    include (FetchContent)

    FetchContent_GetProperties (boost)
    if (NOT boost_POPULATED)
        FetchContent_Declare (boost
                GIT_REPOSITORY https://github.com/boostorg/boost
                GIT_TAG "origin/master"
                GIT_PROGRESS true
                QUIET)
        FetchContent_Populate (boost)

        # The following needs to be run after initial checkout or
        # after the dependency details of boost are changed.
        # The following option is provided to prevent having to
        # keep reconfiguring and rerunning the boost build each
        # time CMake runs once it has been successfully performed
        # for the version specified.
        option (ENABLE_BOOST_BUILD "Enable reconfiguring and rerunning the boost build" ON)
        if (ENABLE_BOOST_BUILD)
            # This file comes from the following location:
            #   https://github.com/pfultz2/cget/blob/master/cget/cmake/boost.cmake
            file (DOWNLOAD "https://raw.githubusercontent.com/pfultz2/cget/master/cget/cmake/boost.cmake" ${CMAKE_BINARY_DIR}/boost.cmake)
            configure_file (${CMAKE_BINARY_DIR}/boost.cmake
                    ${boost_SOURCE_DIR}/CMakeLists.txt
                    COPYONLY
                    )

            unset (generatorArgs)
            set (cacheArgs
                    "-DCMAKE_INSTALL_PREFIX:STRING=${boost_BINARY_DIR}/install"
                    "-DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=YES"
                    "-DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}"
                    )
            if (CMAKE_TOOLCHAIN_FILE)
                list (APPEND cacheArgs "-DCMAKE_TOOLCHAIN_FILE:FILEPATH=${CMAKE_TOOLCHAIN_FILE}")
            else ()
                list (APPEND cacheArgs "-DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}"
                        "-DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}"
                        )
            endif ()

            get_property (isMulti GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
            if (NOT isMulti)
                list (APPEND cacheArgs "-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}")
            endif ()

            if (CMAKE_GENERATOR_PLATFORM)
                list (APPEND generatorArgs
                        --build-generator-platform "${CMAKE_GENERATOR_PLATFORM}"
                        )
            endif ()
            if (CMAKE_GENERATOR_TOOLSET)
                list (APPEND generatorArgs
                        --build-generator-toolset  "${CMAKE_GENERATOR_TOOLSET}"
                        )
            endif ()

            # Assume parent dir has set BOOST_LIBS to a list of the boost modules
            # we want build and made available
            foreach (lib IN LISTS BOOST_LIBS)
                list (APPEND cacheArgs "-DBOOST_WITH_${lib}:STRING=--with-${lib}")
            endforeach ()

            message (STATUS "Configuring and building boost immediately")
            execute_process (
                    COMMAND ${CMAKE_CTEST_COMMAND}
                    --build-and-test  ${boost_SOURCE_DIR} ${boost_BINARY_DIR}
                    --build-generator ${CMAKE_GENERATOR} ${generatorArgs}
                    --build-target    install
                    --build-noclean
                    --build-options   ${cacheArgs}
                    WORKING_DIRECTORY ${boost_SOURCE_DIR}
                    OUTPUT_FILE       ${boost_BINARY_DIR}/build_output.log
                    ERROR_FILE        ${boost_BINARY_DIR}/build_output.log
                    RESULT_VARIABLE   result
            )
            message (STATUS "boost build complete")
            if (result)
                message (FATAL_ERROR "Failed boost build, see build log at:\n"
                        "    ${boost_BINARY_DIR}/build_output.log")
            endif ()

        endif ()
        set (CMAKE_PREFIX_PATH ${boost_BINARY_DIR}/install)
    endif ()

    # Confirm we can find Boost. If this is the first time we've
    # tried to find it in this build dir, this call will force the
    # location of each Boost library to be saved in the cache, so
    # later calls elsewhere in the project will find the same ones.
find_package (Boost QUIET REQUIRED COMPONENTS ${BOOST_LIBS})

endif ()