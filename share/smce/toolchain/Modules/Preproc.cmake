#
#  Preproc.cmake (runtime)
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

function (identify_source SOURCE_FILE SOURCE_TYPE)
    set (SOURCE_FILE_ORIGINAL "${SOURCE_FILE}" PARENT_SCOPE)
    if (SOURCE_TYPE STREQUAL "sketch")
        if (SOURCE_FILE MATCHES "\\.ino$" OR SOURCE_FILE MATCHES "\\.pde$" )
            set (SOURCE_IS_CPP OFF PARENT_SCOPE)
            set (SOURCE_IS_CPP OFF)
            set (SOURCE_IS_INO ON PARENT_SCOPE)
            set (SOURCE_IS_INO ON)
            file (MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/src")
            file (COPY "${SOURCE_FILE}" DESTINATION "${CMAKE_BINARY_DIR}/src")
            get_filename_component (SOURCE_FILENAME "${SOURCE_FILE}" NAME)
            file (RENAME "${CMAKE_BINARY_DIR}/src/${SOURCE_FILENAME}" "${CMAKE_BINARY_DIR}/src/source.ino")
            set (SOURCE_FILE "${CMAKE_BINARY_DIR}/src/source.ino" PARENT_SCOPE)
        else ()
            set (SOURCE_IS_CPP ON PARENT_SCOPE)
            set (SOURCE_IS_CPP ON)
            set (SOURCE_IS_INO OFF PARENT_SCOPE)
            set (SOURCE_IS_INO OFF)
            return ()
        endif ()
    elseif (SOURCE_TYPE STREQUAL "sketch_dir")
        set (SOURCE_IS_CPP OFF PARENT_SCOPE)
        set (SOURCE_IS_CPP OFF)
        set (SOURCE_IS_INO ON PARENT_SCOPE)
        set (SOURCE_IS_INO ON)
    #[[ Not an Arduino feature; to be continued
    elseif (SOURCE_TYPE STREQUAL "sketch_dir_recurse")
        set (SOURCE_IS_CPP OFF PARENT_SCOPE)
        set (SOURCE_IS_CPP OFF)
        set (SOURCE_IS_INO ON PARENT_SCOPE)
        set (SOURCE_IS_INO ON)
        file (GLOB SKETCHES
                FOLLOW_SYMLINKS
                LIST_DIRECTORIES false
                "${SOURCE_FILE}/*.ino" "${SOURCE_FILE}/*.pde")
    ]]
    endif ()

    if (NOT SOURCE_IS_INO)
        message (FATAL_ERROR "Could not determine source file type")
    endif()
endfunction()

function(ino_preprocess SOURCE_FILE ARDUINOCLI_PATH)
    set (WORK_FILE "${CMAKE_BINARY_DIR}/source.cpp")
    set (FQBN "esp32:esp32:esp32")
    execute_process (COMMAND ${ARDUINOCLI_PATH} core update-index --config-file "${CMAKE_SOURCE_DIR}/arduino-cli.yaml")
    execute_process (COMMAND "${ARDUINOCLI_PATH}" core install esp32:esp32 --config-file "${CMAKE_SOURCE_DIR}/arduino-cli.yaml")
    execute_process (
            COMMAND "${ARDUINOCLI_PATH}" compile --config-file "${CMAKE_SOURCE_DIR}/arduino-cli.yaml" --libraries "smartcar_shield" -b "${FQBN}" --preprocess "${SOURCE_FILE}"
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
            OUTPUT_FILE "${WORK_FILE}")

    set (SOURCE_FILE "${WORK_FILE}" PARENT_SCOPE)
endfunction()

function(fetch_ardcli ARDUINOCLI_PATH)
    if (NOT "${ARDUINOCLI_PATH}" STREQUAL "")
        if (NOT EXISTS "${ARDUINOCLI_PATH}")
            message (FATAL_ERROR "Could not find ArduinoCLI at configured location")
        endif ()
        return ()
    elseif (EXISTS "${CMAKE_SOURCE_DIR}/arduino-cli${CMAKE_EXECUTABLE_SUFFIX}")
        set (ARDUINOCLI_PATH "${CMAKE_SOURCE_DIR}/arduino-cli${CMAKE_EXECUTABLE_SUFFIX}" PARENT_SCOPE)
        return ()
    endif ()
    if (WIN32)
        set (DL_URL "https://github.com/arduino/arduino-cli/releases/download/0.9.0/arduino-cli_0.9.0_Windows_64bit.zip")
        set (DL_FILE arduino-cli.zip)
    elseif (APPLE)
        set (DL_URL "https://github.com/arduino/arduino-cli/releases/download/0.9.0/arduino-cli_0.9.0_macOS_64bit.tar.gz")
        set (DL_FILE arduino-cli.tgz)
    else ()
        set (DL_URL "https://github.com/arduino/arduino-cli/releases/download/0.9.0/arduino-cli_0.9.0_Linux_64bit.tar.gz")
        set (DL_FILE arduino-cli.tgz)
    endif ()
    file (DOWNLOAD "${DL_URL}" "${CMAKE_SOURCE_DIR}/${DL_FILE}"
            STATUS DL_STATUS)
    list (GET DL_STATUS 0 DL_RESULT)
    if (NOT DL_RESULT EQUAL 0)
        list (GET "${DL_STATUS}" 1 DL_ERRMSG)
        message (FATAL_ERROR "Downloading ArduinoCLI failed (${DL_RESULT}): ${DL_ERRMSG}")
    endif ()
    execute_process (COMMAND "${CMAKE_COMMAND}" -E tar x "${CMAKE_SOURCE_DIR}/${DL_FILE}"
                     WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
    set (ARDUINOCLI_PATH "${CMAKE_SOURCE_DIR}/arduino-cli${CMAKE_EXECUTABLE_SUFFIX}" PARENT_SCOPE)
    set (ARDUINOCLI_PATH "${CMAKE_SOURCE_DIR}/arduino-cli${CMAKE_EXECUTABLE_SUFFIX}")
    if (EXISTS "${ARDUINOCLI_PATH}")
        message (STATUS "Successfully installed ArduinoCLI")
    else ()
        message (FATAL_ERROR "Installation of ArduinoCLI failed")
    endif ()
    file (REMOVE "${CMAKE_SOURCE_DIR}/${DL_FILE}")
    file (WRITE "${CMAKE_SOURCE_DIR}/arduino-cli.yaml" "board_manager:\n  additional_urls:\n    - https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json\n")
    file (APPEND "${CMAKE_SOURCE_DIR}/arduino-cli.yaml" "directories:\n  user: \"${CMAKE_SOURCE_DIR}\"\n")
    execute_process (COMMAND ${ARDUINOCLI_PATH} core update-index)
endfunction()