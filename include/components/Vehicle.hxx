/*
 *  Vehicle.hxx
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
// Copyright (c) 2008-2020 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
#ifndef SMARTCAR_EMUL_VEHICLE_HXX
#define SMARTCAR_EMUL_VEHICLE_HXX

#include <Urho3D/Input/Controls.h>
#include <Urho3D/Physics/PhysicsUtils.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <array>

class Vehicle : public Urho3D::LogicComponent {
    URHO3D_OBJECT(Vehicle, LogicComponent)

  public:
    explicit Vehicle(Urho3D::Context* context);
    ~Vehicle() override;
    static void RegisterObject(Urho3D::Context* context);
    void Init();
    void PostUpdate(float timeStep);
    float GetWheelRadius() const { return wheelRadius_; }

  private:
    float engineForce_ = 0.0f;
    float maxEngineForce_ = 1.097f;
    float wheelRadius_ = 0.0335f;
    float suspensionRestLength_ = 0.0345f;
    float suspensionStiffness_ = 100.61f;
    float suspensionDamping_ = 8.0f;
    float suspensionCompression_ = 13.1f;
    // the total Friction of wheels equals 10.4272N assuming Friction coefficent is 0.8
    float wheelFriction_ = 2.7f;
    float rollInfluence_ = 0.01f;
    Urho3D::Vector3 prevVelocity_;
    std::array<Urho3D::Vector3, 4> connectionPoints_;
};
#endif // SMARTCAR_EMUL_VEHICLE_HXX