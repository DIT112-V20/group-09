/*
 *  Loader.cxx
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
#include "fmt/format.h"
#include "INIReader.h"
#include "Loader.hxx"
namespace smce {

ProgramOptions LoadProgramOptions(const stdfs::path& path) {
    auto reader = INIReader(path.string());

    if (reader.ParseError() != 0)
        return ProgramOptions{};

    auto read_ini_path = [&](std::string_view section, std::string_view name, std::string_view sub) -> stdfs::path {
        auto tmp = reader.Get(section.data(), name.data(), sub.data());
        return stdfs::exists(tmp) ? tmp : sub;
    };

    ProgramOptions config(read_ini_path("paths", "preprocessor_bin", ProgramOptions::DEFAULT_PREPROCESSOR_BIN),
                          read_ini_path("paths", "smce_home", ProgramOptions::DEFAULT_SMCE_HOME),
                          read_ini_path("paths", "lib_path", ProgramOptions::DEFAULT_LIB_PATH),
                          reader.GetInteger("versions", "cpp_std", ProgramOptions::DEFAULT_CPP_STD));

    return config;
}

void ExportPorgramOptions(const ProgramOptions& co, const stdfs::path& path) {
    if (std::ofstream out(path, std::ios::out | std::ios::binary); out) {
        auto ret = fmt::format("[paths]\npreprocessor_bin={}\nsmce_home={}\nlib_path={}\n[versions]\ncpp_std={}\n", co.preprocessor_bin.string(),
                               co.smce_home.string(), co.lib_path.string(), co.cpp_std);
        out.write(ret.data(), ret.size());
    }
}

} // namespace smce
