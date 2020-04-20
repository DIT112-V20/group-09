#
#  SetupUrho3D.cmake
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

# Sets up the Urho3D target

list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/Urho3D/CMake/Modules")
set (URHO3D_64BIT ON) # We don't want to support 32bit nor require multilib toolchains on 64bit systems

if  (URHO3D_HOME_RELATIVE)
    set (URHO3D_HOME ${CMAKE_CURRENT_SOURCE_DIR}/${URHO3D_HOME_RELATIVE})
endif  (URHO3D_HOME_RELATIVE)

find_package (Urho3D REQUIRED)
add_library (Urho3D INTERFACE)


target_include_directories (Urho3D INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/Urho3D/Source/ThirdParty)
target_include_directories (Urho3D INTERFACE ${URHO3D_INCLUDE_DIRS})

target_link_libraries (Urho3D INTERFACE ${URHO3D_LIBRARIES})
target_compile_options (Urho3D INTERFACE "$<$<CXX_COMPILER_ID:GNU,Clang>:-Wno-invalid-offsetof>")

# Urho3D includes a version of Bullet which is broken on new Apple compilers
# We force-patch the source
if  (APPLE)
    set (BULLET_FILE_TO_PATCH ${URHO3D_HOME}/include/Urho3D/ThirdParty/Bullet/LinearMath/btVector3.h)
    file (READ ${BULLET_FILE_TO_PATCH} BULLET_PATCH_IN)
    string (REPLACE "#define BT_SHUFFLE (x,y,z,w)  ( (w)<<6 |  (z)<<4 |  (y)<<2 |  (x))"
            "#define BT_SHUFFLE (x, y, z, w)  ( ( (w) << 6 |  (z) << 4 |  (y) << 2 |  (x)) & 0xff)"
            BULLET_PATCH_OUT "${BULLET_PATCH_IN}")
    file (WRITE "${BULLET_FILE_TO_PATCH}" "${BULLET_PATCH_OUT}")
    unset (BULLET_FILE_TO_PATCH)
    unset (BULLET_PATCH_IN)
    unset (BULLET_PATCH_OUT)
endif  (APPLE)

if  (WIN32)
    target_compile_definitions (Urho3D INTERFACE URHO3D_WIN32_CONSOLE=1)
    list (APPEND URHO_LIBS Imm32 winmm Version)
    if  (URHO3D_MINIDUMPS)
        list (APPEND LIBS dbghelp)
    endif  ()
elseif  (APPLE)
    if  (ARM)
        list (APPEND URHO_LIBS "-framework AudioToolbox" "-framework AVFoundation" "-framework CoreAudio" "-framework CoreGraphics" "-framework CoreMotion" "-framework Foundation" "-framework GameController" "-framework OpenGLES" "-framework QuartzCore" "-framework UIKit")
    else  ()
        list (APPEND URHO_LIBS "-framework AudioToolbox" "-framework Carbon" "-framework Cocoa" "-framework CoreFoundation" "-framework SystemConfiguration" "-framework CoreAudio" "-framework CoreServices" "-framework CoreVideo" "-framework ForceFeedback" "-framework IOKit" "-framework OpenGL")
    endif  ()
endif  ()

# Graphics
if  (URHO3D_OPENGL)
    if  (APPLE)
        # Do nothing
    elseif  (WIN32)
        list (APPEND URHO_LIBS opengl32)
    else  ()
        list (APPEND URHO_LIBS GL dl)
    endif  ()
elseif  (WIN32)
    set (DIRECTX_REQUIRED_COMPONENTS)
    set (DIRECTX_OPTIONAL_COMPONENTS DInput DSound XAudio2 XInput)
    if  (NOT URHO3D_OPENGL)
        if  (URHO3D_D3D11)
            list (APPEND DIRECTX_REQUIRED_COMPONENTS D3D11)
        else  ()
            list (APPEND DIRECTX_REQUIRED_COMPONENTS D3D)
        endif  ()
    endif  ()
    find_package (DirectX REQUIRED ${DIRECTX_REQUIRED_COMPONENTS} OPTIONAL_COMPONENTS ${DIRECTX_OPTIONAL_COMPONENTS})
    if  (DIRECTX_FOUND)
        target_include_directories (Urho3D INTERFACE SYSTEM ${DIRECTX_INCLUDE_DIRS})
        target_link_directories (Urho3D INTERFACE ${DIRECTX_LIBRARY_DIRS})
    endif  ()
    list (APPEND URHO_LIBS ${DIRECT3D_LIBRARIES})
endif  ()

foreach  (LIB ${URHO_LIBS})
    target_link_libraries (Urho3D INTERFACE ${LIB})
endforeach  ()

add_library (Urho3D::Urho3D ALIAS Urho3D)

add_library (Urho3D-rapidjson INTERFACE)
target_link_libraries (Urho3D-rapidjson INTERFACE Urho3D::Urho3D)
target_include_directories (Urho3D-rapidjson INTERFACE "${CMAKE_SOURCE_DIR}/thirdparty/Urho3D/Source/ThirdParty/rapidjson/include")
