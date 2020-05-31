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
#include <fmt/printf.h>

LaserCaster::LaserCaster(Urho3D::Node* node) : Urho3D::LogicComponent{node->GetContext()}, node{node} {}

[[nodiscard]] std::uint32_t LaserCaster::measure(float max_dist) const noexcept {
    Urho3D::PhysicsRaycastResult ret;
    Urho3D::Ray ray{node->GetWorldPosition(), node->GetWorldDirection()};
    //context_->GetSubsystem<Urho3D::PhysicsWorld>()->RaycastSingle(ret, ray, max_dist, 1);
    context_->GetSubsystem<Urho3D::PhysicsWorld>()->RaycastSingle(ret, ray, 1000, 1);

    //fmt::print("ray: {}\n", ret.distance_);
    //fmt::print("x: {} y: {} z: {} ray: {}\n", ray.direction_.x_, ray.direction_.y_, ray.direction_.z_, ret.distance_);
    //fmt::print("\nYYY x: {} y: {} z: {} ray: {}\n\n", node->GetPosition().x_,node->GetPosition().y_,node->GetPosition().z_, ret.distance_);

    return ret.distance_;
}
