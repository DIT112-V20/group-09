/*
 *  TorchMenu.cxx
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

#include <Urho3D/Core/Object.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/ListView.h>
#include <Urho3D/UI/Text.h>
#include <fmt/format.h>
#include "gui/TorchEvents.hxx"
#include "gui/TorchMenu.hxx"

TorchMenu::TorchMenu(Urho3D::Context* context)
    : Urho3D::LogicComponent(context), ui{GetSubsystem<Urho3D::UI>()} {
    SubscribeToEvent(Urho3D::E_KEYUP, URHO3D_HANDLER(TorchMenu, handle_key));
    SubscribeToEvent(E_INO_COMPILE_RESULT, URHO3D_HANDLER(TorchMenu, handle_compile_result));

    options = smce::get_default_options();

    auto* cache = GetSubsystem<Urho3D::ResourceCache>();

    main_gui = ui->GetRoot()->CreateChild<Urho3D::UIElement>();
    main_gui->SetSize(ui->GetRoot()->GetSize());

    styling = cache->GetResource<Urho3D::XMLFile>("Torch/Data/UI/TorchStyle.xml");

    auto load_layout = [&](std::string path) {
        auto ret = ui->LoadLayout(cache->GetResource<Urho3D::XMLFile>(path.c_str()), styling);
        main_gui->AddChild(ret);
        return ret;
    };

    // Background fill
    background = main_gui->CreateChild<Urho3D::Window>();
    background->SetSize(main_gui->GetSize());
    background->SetDefaultStyle(styling);
    background->SetColor({0.05, 0.05, 0.05, 1});

    // Console
    console = GetSubsystem<Urho3D::Engine>()->CreateConsole();
    console->SubscribeToEvent(console->GetLineEdit(), Urho3D::E_TEXTFINISHED, URHO3D_HANDLER(TorchMenu, handle_console_redirect));
    console->SetDefaultStyle(styling);
    console->GetBackground()->SetOpacity(0.8f);

    // Layouts
    main_menu = load_layout("Torch/Data/UI/TorchOverlay.xml");
    settings_menu = load_layout("Torch/Data/UI/TorchSettings.xml");
    settings_menu->SetVisible(false);

    auto sub_event = [&](auto name, auto ptr) {
        SubscribeToEvent(ui->GetRoot()->GetChild(name, true), Urho3D::E_RELEASED, new Phandler(this, ptr));
    };

    sub_event("exit_button", &TorchMenu::handle_exit);
    sub_event("reset_button", &TorchMenu::handle_reset);
    sub_event("settings_button", &TorchMenu::handle_settings);
    sub_event("back_to_main_button", &TorchMenu::handle_back);
    sub_event("toggle_pause_button", &TorchMenu::handle_pause);

    auto sub_selector_event = [&](auto name, auto ptr, SelectorData dat) {
        auto* sub_ptr = ui->GetRoot()->GetChild(name, true);
        selector_data_map[sub_ptr] = dat;
        SubscribeToEvent(sub_ptr, Urho3D::E_RELEASED, new Phandler(this, ptr));
    };

    sub_selector_event("load_button", &TorchMenu::handle_start, {{".ino"}, "Select a Arduino Sketch file to compile", "Compile"});
    sub_selector_event("smce_home_set_button", smce_home_selector_handlers, {{".$"}, "Select the SMCE home directory"});
    sub_selector_event("veh_conf_set_button", veh_conf_selector_handlers, {{".json"}, "Select the vehicle config file"});
    sub_selector_event("board_conf_set_button", board_conf_selector_handlers, {{".json"}, "Select the board data config file"});

    update_runtime_values();
}

void TorchMenu::update_runtime_values() {
    auto set_text = [&] (auto id, const std::string& str){
      ui->GetRoot()
          ->GetChildDynamicCast<Urho3D::Text>(id, true)
          ->SetText(str.c_str());
    };
    main_gui->GetChildDynamicCast<Urho3D::Text>("sketch_info", true)->SetText(( "  - Sketch: " + (ino_path ? ino_path->filename().generic_string() : "...")).c_str());
    set_text("veh_conf_info0"," " + options.vehicle_config.generic_string());
    set_text("veh_conf_info1","  - Vehicle Conf: " + options.vehicle_config.filename().generic_string());
    set_text("board_conf_info0"," " + options.board_config.generic_string());
    set_text("board_conf_info1","  - Board Conf: " + options.board_config.filename().generic_string());
    set_text("smce_home_set_button_text"," " + std::filesystem::absolute(options.smce_home).generic_string());
}

void TorchMenu::handle_key(Urho3D::StringHash, Urho3D::VariantMap& event_data) {
    const auto key = event_data[Urho3D::KeyUp::P_KEY].GetInt();
    if (key == Urho3D::KEY_F12)
        console->SetVisible(!console->IsVisible());
    if (key == Urho3D::KEY_ESCAPE) {
        if (console->IsVisible())
            console->SetVisible(false);
        else if (emu_state == EmuStates::RUNNING || emu_state == EmuStates::PAUSED) {
            auto* const input = GetSubsystem<Urho3D::Input>();
            input->SetMouseMode(main_gui->IsVisible() ? Urho3D::MM_RELATIVE : Urho3D::MM_ABSOLUTE);
            main_gui->SetVisible(!main_gui->IsVisible());
        }
    }
}

void TorchMenu::handle_compile_result(Urho3D::StringHash, Urho3D::VariantMap& event_data) {
    const auto compiled = event_data[InoCompiled::P_INO_COMPILE_RESULT].GetBool();
    auto* load_button = main_gui->GetChild("load_button", true);
    load_button->SetEnabled(true);
    auto* load_button_text = main_gui->GetChildDynamicCast<Urho3D::Text>("load_button_text", true);
    std::string popup_title{};
    if (compiled) {
        emu_state = EmuStates::RUNNING;
        auto* reset_button = main_gui->GetChild("reset_button", true);
        auto* pause_button = main_gui->GetChild("toggle_pause_button", true);
        pause_button->SetEnabled(true);
        reset_button->SetEnabled(true);
        load_button_text->SetText("Enter World");
        background->SetVisible(false);
        popup_title = "Compile Success";
    } else {
        emu_state = EmuStates::NO_SKETCH;
        load_button_text->SetText("Load");
        popup_title = "Compile Error";
    }
    make_popup(popup_title, event_data[InoCompiled::P_INO_COMPILE_MESSAGE].GetString().CString());
}

void TorchMenu::handle_selector_cancel(Urho3D::StringHash, Urho3D::VariantMap&) { selector.Reset(); }
void TorchMenu::handle_exit(Urho3D::StringHash, Urho3D::VariantMap&) { GetSubsystem<Urho3D::Engine>()->Exit(); }
void TorchMenu::handle_back(Urho3D::StringHash, Urho3D::VariantMap&) { settings_menu->SetVisible(false); }
void TorchMenu::handle_pause(Urho3D::StringHash, Urho3D::VariantMap&) {
    auto* pause_button = main_gui->GetChild("toggle_pause_button", true);
    auto* pause_button_text = main_gui->GetChildDynamicCast<Urho3D::Text>("toggle_pause_button_text", true);
    Urho3D::VariantMap map{};
    if (emu_state == EmuStates::RUNNING) {
        map[InoRuntimeCommand::P_INO_RUNTIME_COMMAND] = static_cast<int>(RuntimeState::PAUSE);
        SendEvent(E_INO_RUNTIME_STATE_COMMAND, map);
        pause_button_text->SetText("Resume");
        emu_state = EmuStates::PAUSED;
    } else if (emu_state == EmuStates::PAUSED) {
        map[InoRuntimeCommand::P_INO_RUNTIME_COMMAND] = static_cast<int>(RuntimeState::RESUME);
        SendEvent(E_INO_RUNTIME_STATE_COMMAND, map);
        pause_button_text->SetText("Pause");
        emu_state = EmuStates::RUNNING;
    }
}
void TorchMenu::handle_settings(Urho3D::StringHash, Urho3D::VariantMap&) { settings_menu->SetVisible(true); }

void TorchMenu::handle_reset(Urho3D::StringHash, Urho3D::VariantMap&) {
    background->SetVisible(true);
    emu_state = EmuStates::NO_SKETCH;
    ui->GetRoot()->GetChild("toggle_pause_button", true)->SetEnabled(false);
    ui->GetRoot()->GetChild("reset_button", true)->SetEnabled(false);
    ui->GetRoot()->GetChild("load_button", true)->SetEnabled(true);
    main_gui->GetChildDynamicCast<Urho3D::Text>("load_button_text", true)->SetText("Load");
    ino_path.reset();
    update_runtime_values();
    Urho3D::VariantMap map{};
    map[InoRuntimeCommand::P_INO_RUNTIME_COMMAND] = static_cast<int>(RuntimeState::RESET);
    SendEvent(E_INO_RUNTIME_STATE_COMMAND, map);
}

void TorchMenu::handle_start(Urho3D::StringHash hash, Urho3D::VariantMap& map) {
    if (std::filesystem::exists(options.vehicle_config) && std::filesystem::exists(options.board_config)) {
        if (emu_state == EmuStates::NO_SKETCH)
            (this->*load_ino_selector_handlers)(hash, map);
        else if (emu_state == EmuStates::RUNNING || emu_state == EmuStates::PAUSED) {
            auto* input = GetSubsystem<Urho3D::Input>();
            main_gui->SetVisible(!main_gui->IsVisible());
            input->SetMouseMode(main_gui->IsVisible() ? Urho3D::MM_ABSOLUTE : Urho3D::MM_RELATIVE);
        }
    } else {
        send_compile_event();
    }
}

void TorchMenu::handle_ino_set(Urho3D::StringHash, Urho3D::VariantMap&) {
    ino_path = get_selector_path();
    update_runtime_values();
    selector.Reset();
    send_compile_event();
}

void TorchMenu::handle_smce_home_set(Urho3D::StringHash, Urho3D::VariantMap&) {
    options.smce_home = get_selector_path().parent_path();
    update_runtime_values();
    selector.Reset();
}

void TorchMenu::handle_veh_conf_set(Urho3D::StringHash, Urho3D::VariantMap&) {
    options.vehicle_config = get_selector_path();
    update_runtime_values();
    selector.Reset();
}

void TorchMenu::handle_board_conf_set(Urho3D::StringHash, Urho3D::VariantMap&) {
    options.board_config = get_selector_path();
    update_runtime_values();
    selector.Reset();
}

void TorchMenu::handle_popup_ok(Urho3D::StringHash, Urho3D::VariantMap&) {
    if (popup)
        popup->Remove();
    popup.Reset();
}

void TorchMenu::make_popup(std::string warn_title, std::string warning) {
    popup = ui->LoadLayout(GetSubsystem<Urho3D::ResourceCache>()->GetResource<Urho3D::XMLFile>("Torch/Data/UI/TorchPopup.xml"), styling);
    popup->SetPriority(10000);
    popup->GetChildDynamicCast<Urho3D::Text>("popup_title", true)->SetText(warn_title.c_str());
    popup->GetChildDynamicCast<Urho3D::Text>("popup_text", true)->SetText(warning.c_str());
    auto* hello = popup->GetChild("ok_button", true);
    SubscribeToEvent(hello, Urho3D::E_CLICKEND, URHO3D_HANDLER(TorchMenu, handle_popup_ok));
    main_gui->AddChild(popup);
}

void TorchMenu::send_compile_event() {
    if (ino_path && std::filesystem::exists(options.vehicle_config) && std::filesystem::exists(options.board_config) &&
        emu_state == EmuStates::NO_SKETCH) {
        auto* load_button = main_gui->GetChild("load_button", true);
        load_button->SetEnabled(false);
        main_gui->GetChildDynamicCast<Urho3D::Text>("load_button_text", true)->SetText("Compiling..");

        Urho3D::VariantMap map;
        // VariantMap requires RefCounted so convert to Urho3D::String
        map[SketchCompileData::P_INO_PATH] = Urho3D::String(ino_path->c_str());
        map[SketchCompileData::P_VEH_CONFIG_PATH] = Urho3D::String(options.vehicle_config.c_str());
        map[SketchCompileData::P_BOARD_CONFIG_PATH] = Urho3D::String(options.board_config.c_str());
        map[SketchCompileData::P_SMCE_HOME_PATH] = Urho3D::String(options.smce_home.c_str());

        SendEvent(E_SKETCH_COMPILE_DATA, map);
    } else {
        make_popup("Compile Error", "Cannot compile without a board and vehicle config.");
    }
}
void TorchMenu::handle_console_redirect(Urho3D::StringHash, Urho3D::VariantMap&) {
    if (auto* line_edit = console->GetLineEdit()) {
        Urho3D::VariantMap map;
        auto text = line_edit->GetText();
        map[TorchUartWrite::P_TORCH_INPUT] = text;
        SendEvent(E_TORCH_UART_WRITE, map);
        line_edit->SetText("");
    }
}
