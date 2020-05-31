/*
 *  ProgramOptions.cxx
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
 *
 */

#include <fstream>
#include <string>
#include <fmt/format.h>
#include <inih/INIReader.h>
#include "ProgramOptions.hxx"

namespace smce {

ProgramOptions LoadProgramOptions(const stdfs::path& path) {
    auto reader = INIReader(path.string());

    if (reader.ParseError() != 0)
        return ProgramOptions{};

    auto read_ini_path = [&](std::string_view section, std::string_view name, stdfs::path sub) -> stdfs::path {
        auto tmp = stdfs::path{reader.Get(section.data(), name.data(), sub.string().data())};
        return stdfs::exists(tmp) ? tmp : sub;
    };

    ProgramOptions config(read_ini_path("paths", "arduinocli_bin", ProgramOptions::DEFAULT_ARDUINOCLI_BIN),
                          read_ini_path("paths", "smce_home", ProgramOptions::DEFAULT_SMCE_HOME),
                          read_ini_path("paths", "vehicle_config", ProgramOptions::DEFAULT_VEHICLE_CONFIG),
                          read_ini_path("paths", "board_config", ProgramOptions::DEFAULT_BOARD_CONFIG),
                          read_ini_path("paths", "lib_path", ProgramOptions::DEFAULT_LIB_PATH));

    return config;
}

void ExportProgramOptions(const ProgramOptions& co, const stdfs::path& path) {
    if (std::ofstream out(path, std::ios::out | std::ios::binary); out) {
        out << fmt::format("[paths]\narduinocli_bin={}\nsmce_home={}\nvehicle_config={}\nboard_config={}\nlib_path={}\n",
                           co.arduinocli_bin.string(), co.smce_home.string(), co.vehicle_config.string(), co.board_config.string(),
                           co.lib_path.string());
    }
}

ProgramOptions get_default_options() {
    std::filesystem::path config_path{};

    if (std::filesystem::exists(smce::ProgramOptions::DEFAULT_CONFIG_FILE_NAME)) {
        config_path = smce::ProgramOptions::DEFAULT_CONFIG_FILE_NAME;
    } else if (auto* env = std::getenv("SMCE_OPTIONS"); env) {
        if (std::filesystem::exists(env))
            config_path = env;
    } else if (auto tmp = smce::PLATFORM_SMCE_HOME / smce::ProgramOptions::DEFAULT_CONFIG_FILE_NAME; std::filesystem::exists(tmp)) {
        config_path = tmp;
    }

    return smce::LoadProgramOptions(config_path);
}

} // namespace smce
