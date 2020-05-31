/*
 *  MovableCamera.cxx
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

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/UI/UI.h>
#include "components/MovableCamera.hxx"

MovableCamera::MovableCamera(Urho3D::Context* context) : Urho3D::Camera{context} {
    SubscribeToEvent(Urho3D::E_UPDATE, URHO3D_HANDLER(MovableCamera, Update));
}

void MovableCamera::RegisterObject(Urho3D::Context* context) { context->RegisterFactory<MovableCamera>(); }

void MovableCamera::Update(Urho3D::StringHash, Urho3D::VariantMap& event_data) {
    const float delta_time = event_data[Urho3D::Update::P_TIMESTEP].GetFloat();
    const auto* const input = GetSubsystem<Urho3D::Input>();
    if (!node_ || GetSubsystem<Urho3D::UI>()->GetFocusElement() || input->IsMouseVisible())
        return;

    const auto [x, y] = input->GetMouseMove();
    m_yaw += mouse_sensitivity * static_cast<float>(x);
    m_pitch += mouse_sensitivity * static_cast<float>(y);
    m_pitch = Urho3D::Clamp(m_pitch, -90.0f, 90.0f);

    node_->SetRotation(Urho3D::Quaternion(m_pitch, m_yaw, 0.0f));

    for (const auto& [key, vec] : key_map) {
        if (input->GetKeyDown(key))
            node_->Translate(vec * move_speed * delta_time);
    }
}
