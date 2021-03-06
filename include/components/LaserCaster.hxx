/*
 *  LaserCaster.hxx
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

#ifndef SMARTCAR_EMUL_LASERCASTER_HXX
#define SMARTCAR_EMUL_LASERCASTER_HXX

#include <cstdint>
#include <Urho3D/Scene/LogicComponent.h>

class LaserCaster : public Urho3D::LogicComponent {
  URHO3D_OBJECT(LaserCaster, Urho3D::LogicComponent);
    Urho3D::Node* node;

  public:
    explicit LaserCaster(Urho3D::Node* node);
    [[nodiscard]] std::uint32_t measure(float max_dist = 1000) const noexcept;
};

#endif // SMARTCAR_EMUL_LASERCASTER_HXX
