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
set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/Catch2/contrib")

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)

if (WIN32)
    add_compile_definitions (VC_EXTRALEAN WIN32_LEAN_AND_MEAN _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING)
endif ()

add_subdirectory (thirdparty/Boost.Assert)
add_subdirectory (thirdparty/Boost.Config)
add_subdirectory (thirdparty/Boost.Core)
add_subdirectory (thirdparty/Boost.Predef)
add_subdirectory (thirdparty/Boost.StaticAssert)
add_subdirectory (thirdparty/Boost.Move)
add_subdirectory (thirdparty/Boost.ThrowException)
add_subdirectory (thirdparty/Boost.TypeTraits)
add_subdirectory (thirdparty/Boost.SmartPtr)
add_subdirectory (thirdparty/Boost.WinAPI)
add_subdirectory (thirdparty/Catch2)
add_subdirectory (thirdparty/fmt)
add_subdirectory (thirdparty/GSL)
add_subdirectory (thirdparty/magic_enum)
add_subdirectory (thirdparty/nameof)

set (RANGES_CXX_STD 20)
set (RANGES_PREFER_REAL_CONCEPTS ON CACHE BOOL "")
set (RANGES_DEEP_STL_INTEGRATION ON CACHE BOOL "")
add_subdirectory (thirdparty/range-v3)

add_custom_target (TestTag)
include (CTest)
include (Catch)
include (SetupUrho3D)
include (SmartcarLib)
include (StdPolyfills)
include (BoostDLL)

add_subdirectory (modules)

execute_process (
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/share" "${CMAKE_BINARY_DIR}/share"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/include/ardpolyfills" "${CMAKE_BINARY_DIR}/share/smce/toolchain/include/ardpolyfills"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/share/smce/toolchain/lib")

add_library (ArduinoRuntime SHARED include/ardpolyfills/SMCE__dll.hxx)
target_link_libraries(ArduinoRuntime PUBLIC Ardrivo)
add_custom_command (TARGET ArduinoRuntime POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:ArduinoRuntime>" "${CMAKE_BINARY_DIR}/share/smce/toolchain/lib/"
        COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_LINKER_FILE:ArduinoRuntime>" "${CMAKE_BINARY_DIR}/share/smce/toolchain/lib/"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/share/smce/toolchain/libraries"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/thirdparty/smartcar_shield" "${CMAKE_BINARY_DIR}/share/smce/toolchain/libraries/smartcar_shield")
file (GENERATE
        OUTPUT "${CMAKE_BINARY_DIR}/share/smce/toolchain/Modules/ArduinoRuntimeLocation.cmake"
        CONTENT "set (ARDUINO_RUNTIME_FILENAME $<TARGET_FILE_NAME:ArduinoRuntime>)\nset (ARDUINO_RUNTIME_LINK_FILENAME $<TARGET_LINKER_FILE_NAME:ArduinoRuntime>)")
add_dependencies(TestTag ArduinoRuntime)


add_library (smartcar_emul_deps INTERFACE)
target_link_libraries (smartcar_emul_deps INTERFACE
        Boost::dll
        fmt::fmt
        Microsoft.GSL::GSL
        magic_enum::magic_enum
        nameof::nameof
        range-v3::range-v3
        Urho3D::Urho3D
        stdpolyfills::stdpolyfills
        TryCompile
        ConfigLoader
        RegMoniker)

add_executable (smartcar_emul src/main.cxx)
target_include_directories (smartcar_emul PRIVATE include thirdparty/inih)
target_link_libraries (smartcar_emul PRIVATE smartcar_emul_deps Urho3D-rapidjson)
target_compile_options (smartcar_emul PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/permissive->)
add_dependencies (smartcar_emul ArduinoRuntime)

target_sources (smartcar_emul PRIVATE
        include/app/UrhoApp.hxx
        src/app/UrhoApp.cxx
        include/components/Registry.hxx
        include/components/MovableCamera.hxx
        src/components/MovableCamera.cxx
        include/UrhoUtility.hxx
        src/gui/TorchMenu.cxx
        include/gui/TorchMenu.hxx
        src/components/EmulGlue.cxx
        include/components/EmulGlue.hxx
        include/gui/TorchEvents.hxx
        include/components/SimpleVehicle.hxx
        src/components/SimpleVehicle.cxx
        src/components/Servo.cxx
        include/components/Servo.hxx
        src/components/WheelServo.cxx
        include/components/WheelServo.hxx
        src/components/PerfectGyroscopeI2CSensor.cxx
        include/components/PerfectGyroscopeI2CSensor.hxx
        src/components/LaserCaster.cxx
        include/components/LaserCaster.hxx
        src/components/PerfectDistanceI2CSensor.cxx
        include/components/PerfectDistanceI2CSensor.hxx)

file (COPY ${CMAKE_SOURCE_DIR}/share DESTINATION ${CMAKE_BINARY_DIR})

set (SMCE_SHARE ${CMAKE_BINARY_DIR}/share/smce)

if (EXISTS ${URHO3D_HOME}/share/Urho3D/Resources)
    set (URHO3D_RESOURCES ${URHO3D_HOME}/share/Urho3D/Resources)
else ()
    set (URHO3D_RESOURCES ${URHO3D_HOME}/bin)
endif ()

execute_process (
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${SMCE_SHARE}/Resources"
        COMMAND "${CMAKE_COMMAND}" -E copy_directory "${URHO3D_RESOURCES}/Autoload" "${SMCE_SHARE}/Resources/Autoload"
        COMMAND "${CMAKE_COMMAND}" -E copy_directory "${URHO3D_RESOURCES}/CoreData" "${SMCE_SHARE}/Resources/CoreData"
        COMMAND "${CMAKE_COMMAND}" -E copy_directory "${URHO3D_RESOURCES}/Data" "${SMCE_SHARE}/Resources/Data")

add_executable (smartcar_emul_tests test/sanity.cxx)
add_dependencies(smartcar_emul_tests TestTag)
target_link_libraries (smartcar_emul_tests PRIVATE
        smartcar_emul_deps
        Catch2)


catch_discover_tests (smartcar_emul_tests)
add_custom_command(TARGET smartcar_emul_tests POST_BUILD COMMAND ctest --output-on-failure)
