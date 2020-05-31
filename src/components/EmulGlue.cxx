/*
 *  EmulGlue.cxx
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

#include <Urho3D/IO/Log.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <fmt/printf.h>
#include <range/v3/algorithm.hpp>
#include "components/EmulGlue.hxx"
#include "components/Registry.hxx"
#include "gui/TorchEvents.hxx"
#include "BoardConf.hxx"
#include "ConfHelper.hxx"
#include "Toolchain.hxx"
#include "VehicleConf.hxx"

EmulGlue::EmulGlue(Urho3D::Context* context) : LogicComponent(context), m_vehicle_node{} {
    SubscribeToEvent(E_SKETCH_COMPILE_DATA, URHO3D_HANDLER(EmulGlue, handle_compile_order));
    SubscribeToEvent(E_INO_RUNTIME_STATE_COMMAND, URHO3D_HANDLER(EmulGlue, handle_runtime_command));
    SubscribeToEvent(E_TORCH_UART_WRITE, URHO3D_HANDLER(EmulGlue, handle_uart_write));
}

EmulGlue::~EmulGlue() { reset_sketch(); }

void EmulGlue::set_byte_write(bool (*write_byte)(unsigned char), size_t (*write_buf)(const unsigned char*, size_t)) {
    m_bdata.write_byte = write_byte;
    m_bdata.write_buf = write_buf;
}

bool EmulGlue::load_config(std::filesystem::path config_path) {
    auto vehicle_conf_opt = smce::load(config_path);
    if (!vehicle_conf_opt)
        return false;
    m_bdata = smce::as_data(*vehicle_conf_opt);
    m_binfo = smce::as_info(*vehicle_conf_opt);

    set_byte_write(
        +[](unsigned char c) {
            Urho3D::String msg{reinterpret_cast<const char*>(&c), 1};
            Urho3D::Log::WriteRaw(msg);
            return true;
        },
        +[](const unsigned char* buf, std::size_t len) {
            Urho3D::String msg(reinterpret_cast<const char*>(buf), len);
            Urho3D::Log::WriteRaw(msg);
            return len;
        });
    return true;
}

void EmulGlue::setup_attachments(BoardData& board, const smce::VehicleConfig& vconf) {
    m_vehicle_attachments.clear();
    for (const auto& [type_str, jconf] : vconf.attachments) {
        const auto& type = type_str;
        auto it = ranges::find_if(attachments_registry, [&](const auto& pair) { return pair.first == type; });
        if (it == attachments_registry.end()) {
            Urho3D::Log::Write(Urho3D::LOG_WARNING, Urho3D::String("Unknown component type ") + type_str.c_str());
            continue;
        }

        try {
            auto* const att = it->second(board, m_vehicle_node.Get(), *jconf);
            m_vehicle_attachments.push_back(att);
            m_vehicle_node->AddComponent(att, 0, Urho3D::REPLICATED);
        } catch (const std::exception& e) {
            Urho3D::Log::Write(Urho3D::LOG_WARNING, Urho3D::String(e.what()) + type_str.c_str());
        }
    }
}

void EmulGlue::reset_sketch() {
    m_ino_runtime.pause_on_next_loop();
    if (m_vehicle_node) {
        m_vehicle_node->RemoveAllChildren();
        m_vehicle_node->RemoveAllComponents();
        m_vehicle_node.Reset();
    }
    m_ino_runtime.clear();
    m_vehicle_attachments.clear();
    m_vehicle.Reset();

}

void EmulGlue::handle_compile_order(Urho3D::StringHash, Urho3D::VariantMap& event_data) {
    auto convert = [&](Urho3D::StringHash hash) -> std::filesystem::path { return event_data[hash].GetString().CString(); };
    auto paths_exist = []<typename... T>(T... t) { return (... && std::filesystem::exists(t)); };

    auto smce_home = convert(SketchCompileData::P_SMCE_HOME_PATH);
    auto b_conf_path = convert(SketchCompileData::P_BOARD_CONFIG_PATH);
    auto veh_conf_path = convert(SketchCompileData::P_VEH_CONFIG_PATH);
    auto ino_path = convert(SketchCompileData::P_INO_PATH);

    if (!paths_exist(smce_home, b_conf_path, veh_conf_path, ino_path)) {
        send_compile_result(false, "Given paths dont exist");
        return;
    }

    auto veh_config_opt = smce::VehicleConfig::load(veh_conf_path);
    if (!load_config(b_conf_path) && !veh_config_opt) {
        send_compile_result(false, "Parsing configs failed");
        return;
    }

    if (!m_vehicle_node)
        m_vehicle_node = node_->CreateChild("EmulGlueVehicle");

    m_vehicle = m_vehicle_node->CreateComponent<SimpleVehicle>();
    m_vehicle->Init();
    setup_attachments(m_bdata, *veh_config_opt);

    m_compile_tr = std::async([&, ino_path, smce_home]() {
        auto ret = smce::compile_sketch({ino_path}, smce_home);
        m_compile_done = true;
        return ret;
    });
}

void EmulGlue::Update(float timestep) {
    if (m_compile_done) {
        auto compile_result = m_compile_tr->get();
        auto err = std::visit(Visitor{
                                  [&](smce::SketchObject& obb) -> std::optional<std::runtime_error> {
                                      m_ino_runtime.set_sketch_and_car(obb, m_bdata, m_binfo);
                                      if (!m_ino_runtime.start())
                                          return std::runtime_error{"Sketch could not be run"};
                                      return {};
                                  },
                                  [&](std::runtime_error& err) -> std::optional<std::runtime_error> { return err; },
                              },
                              compile_result);

        if (err) {
            send_compile_result(false, err->what());
        } else {
            send_compile_result(true, "Compilation has succeeded :)");
        }
        m_compile_done = false;
        m_compile_tr.reset();
    }
}

void EmulGlue::handle_runtime_command(Urho3D::StringHash, Urho3D::VariantMap& event_data) {
    const auto command = static_cast<RuntimeState>(event_data[InoRuntimeCommand::P_INO_RUNTIME_COMMAND].GetUInt());
    switch (command) {
    case RuntimeState::PAUSE:
        m_ino_runtime.pause_now();
        break;
    case RuntimeState::RESUME:
        m_ino_runtime.resume();
        break;
    case RuntimeState::RESET:
        reset_sketch();
        break;
    }
}

void EmulGlue::send_compile_result(bool result, std::string message) {
    Urho3D::VariantMap map{};
    map[InoCompiled::P_INO_COMPILE_RESULT] = result;
    map[InoCompiled::P_INO_COMPILE_MESSAGE] = message.c_str();
    SendEvent(E_INO_COMPILE_RESULT, map);
}

void EmulGlue::handle_uart_write(Urho3D::StringHash, Urho3D::VariantMap& event_data) {
    if (m_ino_runtime.is_running() && !m_bdata.uart_buses.empty()) {
        std::string str = event_data[TorchUartWrite::P_TORCH_INPUT].GetString().CString();
        if (str.back() == '\n')
            str.pop_back();

        std::scoped_lock l{m_bdata.uart_buses[0].rx_mutex};
        const auto original_buf_len = m_bdata.uart_buses[0].rx.size();
        m_bdata.uart_buses[0].rx.resize(m_bdata.uart_buses[0].rx.size() + str.size());
        std::transform(str.begin(), str.end(), m_bdata.uart_buses[0].rx.begin() + original_buf_len, [](char c) { return static_cast<std::byte>(c); });
    }
}
