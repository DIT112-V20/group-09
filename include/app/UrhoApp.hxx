/*
 *  UrhoApp.hxx
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

#ifndef SMARTCAR_EMUL_URHOAPP_HXX
#define SMARTCAR_EMUL_URHOAPP_HXX

#include <vector>
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Scene/Scene.h>

struct BoardData;

namespace smce {
struct VehicleConfig;
}

class UrhoApp : public Urho3D::Application {
    URHO3D_OBJECT(UrhoApp, Urho3D::Application);

  public:
    explicit UrhoApp(Urho3D::Context* context);

    /**
    * Setup before engine initialization
    **/
    void Setup() override;

    /**
    * Setup after engine initialization and before running the main loop
    **/
    void Start() override;
    /**
    * Cleanup after the main loop
    **/
    void Stop() override;

  private:
    enum class Menu {
        none,
        main,
    };

    /**
    * Creates the scene of urho
    **/
    void create_scene();

    /**
    * Implements the actual rendering of 3D views each frame
    **/
    void create_viewport();

    /**
    * Sets the vehicle attatchments and adds the component
    **/
    void setup_attachments(BoardData& board, const smce::VehicleConfig&);

    /**
    * Subscribes to all input, updates and mouseButtonDown events
    **/
    void subscribe_to_events();
    
    /**
    * Handles all application-wide update event
    **/
    void HandleUpdate(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data);

    /**
    * Handles input events
    **/
    void HandleKeyUp(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data);
    
    /**
    * Handle mouse button down event
    **/
    void HandleMouseButtonDown(Urho3D::StringHash, Urho3D::VariantMap&);

    /// Shared pointer for the scene
    Urho3D::SharedPtr<Urho3D::Scene> m_scene;
    /// Shared pointer for the camera
    Urho3D::SharedPtr<Urho3D::Node> m_camera_node;

    /// Shared pointer for the vehicle
    Urho3D::SharedPtr<Urho3D::Node> m_vehicle_node;
    std::vector<Urho3D::LogicComponent*> m_vehicle_attachments;
};

#endif // SMARTCAR_EMUL_URHOAPP_HXX
