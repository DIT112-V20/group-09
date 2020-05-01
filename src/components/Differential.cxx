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
#include "components/Differential.hxx"


Differential::Differential(Urho3D::Context* context) : Urho3D::RaycastVehicle(context) {}

Differential::~Differential() {}

void Differential::setDifferential(float engineForce, const unsigned int control) {
    // failed attempts
    // auto* vehicle = node_->GetComponent<RaycastVehicle>();
    // RaycastVehicle* vehicle = node_->GetComponent<RaycastVehicle>();
    if (control == 8) {
        engineForce = engineForce * -2;
    } else {
        engineForce = engineForce * 2;
    }
    Urho3D::RaycastVehicle::SetEngineForce(0, engineForce);
    Urho3D::RaycastVehicle::SetEngineForce(1, engineForce);
    Urho3D::RaycastVehicle::SetEngineForce(2, -engineForce);
    Urho3D::RaycastVehicle::SetEngineForce(3, -engineForce);
}
