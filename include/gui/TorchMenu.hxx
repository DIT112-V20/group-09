/*
 *  TorchMenu.hxx
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

#ifndef SMARTCAR_EMUL_TORCHMENU_HXX
#define SMARTCAR_EMUL_TORCHMENU_HXX

#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Engine/Console.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/FileSelector.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Window.h>
#include <range/v3/algorithm.hpp>
#include "UrhoUtility.hxx"
#include "ProgramOptions.hxx"

class TorchMenu : public Urho3D::LogicComponent {
    URHO3D_OBJECT(TorchMenu, Urho3D::LogicComponent);

    using Phandler = Urho3D::EventHandlerImpl<TorchMenu>;

    Urho3D::XMLFile* styling;
    Urho3D::SharedPtr<Urho3D::FileSelector> selector;
    Urho3D::SharedPtr<Urho3D::UIElement> popup;

    smce::ProgramOptions options{};
    std::optional<std::filesystem::path> ino_path{};

    struct SelectorData {
        std::vector<std::string> file_filters;
        std::string window_name, ok_text = "Select", cancel_text = "Cancel";
    };
    std::unordered_map<Urho3D::UIElement*, SelectorData> selector_data_map;

    Urho3D::UI* ui;
    Urho3D::UIElement* main_gui;
    Urho3D::UIElement* settings_menu;
    Urho3D::UIElement* main_menu;
    Urho3D::Window* background;
    Urho3D::Console* console;

    enum class EmuStates {
        NO_SKETCH,
        RUNNING,
        PAUSED
    };

    EmuStates emu_state = EmuStates::NO_SKETCH;

    void handle_key(Urho3D::StringHash, Urho3D::VariantMap& event_data);
    void handle_compile_result(Urho3D::StringHash, Urho3D::VariantMap& event_data);
    void handle_ino_set(Urho3D::StringHash, Urho3D::VariantMap&);
    void handle_selector_cancel(Urho3D::StringHash, Urho3D::VariantMap&);
    void handle_exit(Urho3D::StringHash, Urho3D::VariantMap&);
    void handle_back(Urho3D::StringHash, Urho3D::VariantMap&);
    void handle_pause(Urho3D::StringHash, Urho3D::VariantMap&);
    void handle_settings(Urho3D::StringHash, Urho3D::VariantMap&);
    void handle_reset(Urho3D::StringHash, Urho3D::VariantMap&);
    void handle_start(Urho3D::StringHash, Urho3D::VariantMap&);
    void handle_smce_home_set(Urho3D::StringHash, Urho3D::VariantMap&);
    void handle_veh_conf_set(Urho3D::StringHash, Urho3D::VariantMap&);
    void handle_board_conf_set(Urho3D::StringHash, Urho3D::VariantMap&);
    void handle_popup_ok(Urho3D::StringHash, Urho3D::VariantMap&);
    void handle_console_redirect(Urho3D::StringHash, Urho3D::VariantMap&);

    using handle_func = void (TorchMenu::*)(Urho3D::StringHash, Urho3D::VariantMap& event_data);
    template <handle_func CancelHandler, handle_func OkHandler> void handle_get_path(Urho3D::StringHash, Urho3D::VariantMap& event_data) {
        auto* el = static_cast<Urho3D::UIElement*>(event_data[Urho3D::Released::P_ELEMENT].GetPtr());
        if (!selector_data_map.contains(el))
            return;

        auto& dat = selector_data_map[el];

        selector = Urho3D::MakeShared<Urho3D::FileSelector>(context_);
        selector->GetWindow()->SetPriority(999);
        selector->SetDefaultStyle(styling);
        selector->SetButtonTexts(dat.ok_text.c_str(), dat.cancel_text.c_str());
        selector->SetTitle(dat.window_name.c_str());

        auto filter = Urho3D::Vector<Urho3D::String>{};
        ranges::for_each(dat.file_filters, [&](const auto& x) { filter.Push(x.c_str()); });
        selector->SetFilters(filter, 0);

        SubscribeToEvent(selector->GetCancelButton(), Urho3D::E_RELEASED, new Urho3D::EventHandlerImpl<TorchMenu>(this, CancelHandler));
        SubscribeToEvent(selector->GetOKButton(), Urho3D::E_RELEASED, new Urho3D::EventHandlerImpl<TorchMenu>(this, OkHandler));
    }

    constexpr static handle_func load_ino_selector_handlers = &TorchMenu::handle_get_path<&TorchMenu::handle_selector_cancel, &TorchMenu::handle_ino_set>;
    constexpr static handle_func smce_home_selector_handlers = &TorchMenu::handle_get_path<&TorchMenu::handle_selector_cancel, &TorchMenu::handle_smce_home_set>;
    constexpr static handle_func veh_conf_selector_handlers = &TorchMenu::handle_get_path<&TorchMenu::handle_selector_cancel, &TorchMenu::handle_veh_conf_set>;
    constexpr static handle_func board_conf_selector_handlers = &TorchMenu::handle_get_path<&TorchMenu::handle_selector_cancel, &TorchMenu::handle_board_conf_set>;

    void send_compile_event();
    void make_popup(std::string warn_title, std::string warning);
    std::filesystem::path get_selector_path() {
        if (!selector)
            return {};
        std::filesystem::path path{selector->GetPath().CString()};
        path += selector->GetFileName().CString();
        return path;
    }

  public:
    TorchMenu(Urho3D::Context* context);
    static void RegisterObject(Urho3D::Context* context) { context->RegisterFactory<TorchMenu>(); }
    void update_runtime_values();
};

#endif // SMARTCAR_EMUL_TORCHMENU_HXX
