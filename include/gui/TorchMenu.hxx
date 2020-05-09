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
#include <string>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Engine/Console.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/FileSelector.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/ListView.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Window.h>
#include <fmt/printf.h>
#include <range/v3/algorithm.hpp>
#include "UrhoUtility.hxx"

class TorchMenu : public Urho3D::LogicComponent {
    URHO3D_OBJECT(TorchMenu, Urho3D::LogicComponent);

    Urho3D::SharedPtr<Urho3D::UIElement> middle_menu;
    Urho3D::SharedPtr<Urho3D::UIElement> top_text;

    Urho3D::XMLFile* styling;
    Urho3D::SharedPtr<Urho3D::FileSelector> selector;
    Urho3D::UI* ui;
    Urho3D::Window* background;
    Urho3D::Console* console;

    int emu_state = 0;

  public:
    static void RegisterObject(Urho3D::Context* context) { context->RegisterFactory<TorchMenu>(); }

    TorchMenu(Urho3D::Context* context) : Urho3D::LogicComponent(context) {
        SubscribeToEvent(Urho3D::E_KEYUP, URHO3D_HANDLER(TorchMenu, handle_key));
        ui = GetSubsystem<Urho3D::UI>();
        auto* const cache = GetSubsystem<Urho3D::ResourceCache>();

        styling = cache->GetResource<Urho3D::XMLFile>("Torch/Data/UI/TorchStyle.xml");

        auto load_layout = [&](std::string path) {
            auto ret = ui->LoadLayout(cache->GetResource<Urho3D::XMLFile>(path.c_str()), styling);
            ui->GetRoot()->AddChild(ret);
            return ret;
        };

        // Background fill
        background = ui->GetRoot()->CreateChild<Urho3D::Window>();
        background->SetSize(ui->GetRoot()->GetSize());
        background->SetColor(Urho3D::Color(0.1, 0.1, 0.1, 1));
        background->SetColor(Urho3D::Corner::C_TOPLEFT, Urho3D::Color(0.1, 0.1, 0.3, 2));
        background->SetColor(Urho3D::Corner::C_TOPRIGHT, Urho3D::Color(0.2, 0.1, 0.1, 2));

        // Console
        console = GetSubsystem<Urho3D::Engine>()->CreateConsole();
        console->SetVisible(false);
        console->SetDefaultStyle(styling);
        console->GetBackground()->SetOpacity(0.8f);

        // Layouts
        middle_menu = load_layout("Torch/Data/UI/TorchUI.xml");
        top_text = load_layout("Torch/Data/UI/TorchUI2.xml");

        selector = Urho3D::MakeShared<Urho3D::FileSelector>(context_);
        selector->SetDefaultStyle(styling);
        selector->GetWindow()->SetName("file_window");
        selector->GetWindow()->SetVisible(false);
        selector->SetButtonTexts("Load", "Cancel");
        selector->SetTitle("Pick an .ino file to load");
        auto filter = Urho3D::Vector<Urho3D::String>{};
        filter.Push(".ino");
        selector->SetFilters(filter, 0);
        middle_menu->AddChild(selector->GetWindow());

        // Event handling
        SubscribeToEvent(selector->GetOKButton(), Urho3D::E_RELEASED, URHO3D_HANDLER(TorchMenu, handle_selector_ok));
        SubscribeToEvent(selector->GetCancelButton(), Urho3D::E_RELEASED, URHO3D_HANDLER(TorchMenu, handle_selector_cancel));

        auto* start_button = middle_menu->GetChild("start_button", true);
        SubscribeToEvent(start_button, Urho3D::E_RELEASED, URHO3D_HANDLER(TorchMenu, handle_start));

        auto* settings_button = middle_menu->GetChild("settings_button", true);
        SubscribeToEvent(settings_button, Urho3D::E_RELEASED, URHO3D_HANDLER(TorchMenu, handle_exit));

        auto* exit_button = middle_menu->GetChild("exit_button", true);
        SubscribeToEvent(exit_button, Urho3D::E_RELEASED, URHO3D_HANDLER(TorchMenu, handle_exit));
    }

    virtual void Update(float timeStep) override {}

  private:
    void handle_key(Urho3D::StringHash hash, Urho3D::VariantMap& event_data) {
        const auto key = event_data[Urho3D::KeyUp::P_KEY].GetInt();
        if (key == Urho3D::KEY_F12)
            console->SetVisible(!console->IsVisible());
    }
    void handle_selector_ok(Urho3D::StringHash, Urho3D::VariantMap&) {
        selector->GetWindow()->SetVisible(false);
        std::filesystem::path ino_file{selector->GetPath().CString()};
        ino_file += selector->GetFileName().CString();
        if (std::filesystem::exists(ino_file) && ino_file.extension() == ".ino") {
            // TODO: actually do things
            ui->GetRoot()->GetChild("start_button", true)->GetChildDynamicCast<Urho3D::Text>(0)->SetText("compile .ino");
        }
    }
    void handle_selector_cancel(Urho3D::StringHash, Urho3D::VariantMap&) { selector->GetWindow()->SetVisible(false); }
    void handle_exit(Urho3D::StringHash, Urho3D::VariantMap&) { GetSubsystem<Urho3D::Engine>()->Exit(); }
    void handle_start(Urho3D::StringHash, Urho3D::VariantMap&) {
        if (emu_state == 0) {
            selector->GetWindow()->SetVisible(true);
        } else if (emu_state == 1) {
            auto* input = GetSubsystem<Urho3D::Input>();
            ui->GetRoot()->SetVisible(!ui->GetRoot()->IsVisible());
            input->SetMouseMode(ui->GetRoot()->IsVisible() ? Urho3D::MM_ABSOLUTE : Urho3D::MM_RELATIVE);
        }
    }
};

#endif // SMARTCAR_EMUL_TORCHMENU_HXX
