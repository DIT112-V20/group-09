/*
 *  MovableCamera.hxx
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

#ifndef SMARTCAR_EMUL_MOVABLECAMERA_HXX
#define SMARTCAR_EMUL_MOVABLECAMERA_HXX
#include <array>
#include <unordered_map>
#include <utility>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Scene/Component.h>
#include <Urho3D/Scene/Scene.h>

#define DEFAULT_MOVE_SPEED 5.0
#define DEFAULT_MOVE_SENSITIVITY 0.1

#define DEFAULT_FORWARD Urho3D::KEY_W
#define DEFAULT_BACK Urho3D::KEY_S
#define DEFAULT_LEFT Urho3D::KEY_A
#define DEFAULT_RIGHT Urho3D::KEY_D

class MovableCamera : public Urho3D::Camera {
    URHO3D_OBJECT(MovableCamera, Camera);

    float move_speed = DEFAULT_MOVE_SPEED;
    float mouse_sensitivity = DEFAULT_MOVE_SENSITIVITY;

    std::array<std::pair<int, const Urho3D::Vector3>, 4> key_map{{{DEFAULT_FORWARD, Urho3D::Vector3::FORWARD},
                                                                  {DEFAULT_BACK, Urho3D::Vector3::BACK},
                                                                  {DEFAULT_LEFT, Urho3D::Vector3::LEFT},
                                                                  {DEFAULT_RIGHT, Urho3D::Vector3::RIGHT}}};

    float m_yaw = 0.f;
    float m_pitch = 0.f;

  public:
    enum struct MoveKey { FORWARD, BACK, LEFT, RIGHT };

    explicit MovableCamera(Urho3D::Context* context);
    static void RegisterObject(Urho3D::Context* context);
    void Update(Urho3D::StringHash, Urho3D::VariantMap& event_data);
    void setKey(MoveKey key, int new_key) { key_map[static_cast<int>(key)].first = new_key; }
    void unsetKey(MoveKey key) { key_map[static_cast<int>(key)].first = -1; }
};
#endif // SMARTCAR_EMUL_MOVABLECAMERA_HXX
