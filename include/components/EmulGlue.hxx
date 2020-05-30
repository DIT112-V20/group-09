/*
 *  EmulGlue.hxx
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

#ifndef SMARTCAR_EMUL_EMULGLUE_HXX
#define SMARTCAR_EMUL_EMULGLUE_HXX
#include <future>
#include <iostream>
#include <istream>
#include <thread>
#include <vector>
#include <BoardData.hxx>
#include <BoardInfo.hxx>
#include <Runtime.hxx>
#include <Source.hxx>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Scene/Scene.h>
#include <VehicleConf.hxx>
#include "components/SimpleVehicle.hxx"
#include "Toolchain.hxx"
#include "utility.hxx"

class EmulGlue : public Urho3D::LogicComponent {
    URHO3D_OBJECT(EmulGlue, Urho3D::LogicComponent);

    Urho3D::SharedPtr<Urho3D::Node> m_vehicle_node;
    std::vector<Urho3D::LogicComponent*> m_vehicle_attachments;
    Urho3D::SharedPtr<SimpleVehicle> m_vehicle;

    BoardData m_bdata;
    BoardInfo m_binfo;

    smce::SketchRuntime m_ino_runtime{};

    CopyOnMoveAtomic<bool> m_compile_done = false;
    std::optional<std::future<smce::CompilationResults>> m_compile_tr{};

    void handle_compile_order(Urho3D::StringHash, Urho3D::VariantMap& event_data);
    void handle_runtime_command(Urho3D::StringHash, Urho3D::VariantMap& event_data);
    void handle_uart_write(Urho3D::StringHash, Urho3D::VariantMap& event_data);
    void send_compile_result(bool result, std::string message = "Compile Error");

    void set_byte_write(bool (*write_byte)(unsigned char), size_t (*write_buf)(const unsigned char*, size_t));
    void setup_attachments(BoardData& board, const smce::VehicleConfig& vconf);
    bool load_config(std::filesystem::path config_path);
    void reset_sketch();
    void Update(float timestep) override;

  public:
    explicit EmulGlue(Urho3D::Context* context);
    ~EmulGlue();
    static void RegisterObject(Urho3D::Context* context) { context->RegisterFactory<EmulGlue>(); }
};

#endif // SMARTCAR_EMUL_EMULGLUE_HXX
