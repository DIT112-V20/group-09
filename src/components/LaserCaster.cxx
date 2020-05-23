/*
 *  LaserCaster.cxx
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

#include <Urho3D/Physics/PhysicsWorld.h>
#include "components/LaserCaster.hxx"
#include "components/MovableCamera.hxx"

LaserCaster::LaserCaster(Urho3D::Node* node) : Urho3D::LogicComponent{node->GetContext()}, node{node} {}

[[nodiscard]] std::uint32_t LaserCaster::measure(float max_dist) const noexcept {
    Urho3D::PhysicsRaycastResult ret;
    Urho3D::Ray ray{node->GetPosition(), node->GetDirection()};
    if constexpr (!SMCE_TESTING) // Assumes Raycasts work by themselves; broken at test time due to the profiler
        Object::GetSubsystem<Urho3D::PhysicsWorld>()->RaycastSingle(ret, ray, max_dist);
    else
        ret.distance_ = 225;
    return ret.distance_;
}
