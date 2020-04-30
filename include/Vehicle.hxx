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
//======================================================================================
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

#include <Urho3D/Input/Controls.h>
#include <Urho3D/Physics/PhysicsUtils.h>
#include <Urho3D/Scene/LogicComponent.h>

namespace Urho3D {
class Constraint;
class Node;
class RigidBody;
}

using namespace Urho3D;

const unsigned CTRL_FORWARD = (1u << 0u);
const unsigned CTRL_BACK = (1u << 1u);
const unsigned CTRL_LEFT = (1u << 2u);
const unsigned CTRL_RIGHT = (1u << 3u);
const float YAW_SENSITIVITY = 0.1f;
const float ENGINE_POWER = 10.0f;
const float MAX_WHEEL_ANGLE = 22.5f;

class Vehicle : public LogicComponent {
    URHO3D_OBJECT(Vehicle, LogicComponent)

  public:
    explicit Vehicle(Context* context);
    ~Vehicle() override;
    static void RegisterObject(Context* context);
    void Init();
    void FixedUpdate(float timeStep) override;
    void PostUpdate(float timeStep) override;

    /// Setting Differancial. NOT FINAL VERSION. IT WILL BE IN ITS OWN CLASS
    void resetDifferential();
    void setDifferential(float engineForce, const unsigned int control);

    Controls controls_;

    /// Get wheel radius.
    float GetWheelRadius() { return wheelRadius_; }
    /// Get wheel width.
    float GetWheelWidth() { return wheelWidth_; }
  private:
    /// Linear momentum supplied by engine to RigidBody
    float engineForce_;
    /// Maximum linear momentum supplied by engine to RigidBody
    float maxEngineForce_;
    /// Stored wheel radius
    float wheelRadius_;
    /// Suspension rest length (in meters)
    float suspensionRestLength_;
    /// Width of wheel (used only in calculation of wheel placement)
    float wheelWidth_;
    /// Suspension stiffness
    float suspensionStiffness_;
    /// Suspension damping
    float suspensionDamping_;
    /// Suspension compression
    float suspensionCompression_;
    /// Wheel friction
    float wheelFriction_;
    /// Wheel roll influence (how much car will turn sidewise)
    float rollInfluence_;
    /// Value to calculate acceleration.
    Vector3 prevVelocity_;
    /// Storing points for emitters
    Vector3 connectionPoints_[4];
};