/*
 *  TorchEvents.hxx
 *  Copyright 2020 ItJustWorksTM
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef SMARTCAR_EMUL_TORCHEVENTS_HXX
#define SMARTCAR_EMUL_TORCHEVENTS_HXX

#include <Urho3D/Core/Object.h>

/*
 * Event sent out by the GUI (or any other entity),
 * used to give the listener paths were it can find the necessary files to compile a sketch.
 * It's the listeners' responsibility to check if these files actually exist or are useful.
 */
URHO3D_EVENT(E_SKETCH_COMPILE_DATA, SketchCompileData) {
    URHO3D_PARAM(P_INO_PATH, ino_path);
    URHO3D_PARAM(P_VEH_CONFIG_PATH, veh_config_path);
    URHO3D_PARAM(P_BOARD_CONFIG_PATH, board_config_path);
    URHO3D_PARAM(P_SMCE_HOME_PATH, smce_home_path);
}

URHO3D_EVENT(E_INO_COMPILE_RESULT, InoCompiled) {
    URHO3D_PARAM(P_INO_COMPILE_RESULT, compiled);
    URHO3D_PARAM(P_INO_COMPILE_MESSAGE, error);
}

enum class RuntimeState { PAUSE, RESUME, RESET };

URHO3D_EVENT(E_INO_RUNTIME_STATE_COMMAND, InoRuntimeCommand) { URHO3D_PARAM(P_INO_RUNTIME_COMMAND, command); }

URHO3D_EVENT(E_TORCH_UART_WRITE, TorchUartWrite) { URHO3D_PARAM(P_TORCH_INPUT, input); }

#endif // SMARTCAR_EMUL_TORCHEVENTS_HXX
