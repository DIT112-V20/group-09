/*
 *  ProgramOptions.hxx
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

#ifndef SMARTCAR_EMUL_PROGRAMOPTIONS_HXX
#define SMARTCAR_EMUL_PROGRAMOPTIONS_HXX
#include <filesystem>

namespace smce {
namespace stdfs = std::filesystem;

struct ProgramOptions {
    static constexpr auto DEFAULT_PREPROCESSOR_BIN = "";
    static constexpr auto DEFAULT_SMCE_HOME = ".";
    static constexpr auto DEFAULT_LIB_PATH = ".";
    static constexpr int DEFAULT_CPP_STD = 17;

    stdfs::path preprocessor_bin = DEFAULT_PREPROCESSOR_BIN;
    stdfs::path smce_home = DEFAULT_SMCE_HOME;
    stdfs::path lib_path = DEFAULT_LIB_PATH;
    int cpp_std = DEFAULT_CPP_STD;

    ProgramOptions(stdfs::path preprocessor_bin, stdfs::path smce_home, stdfs::path lib_path, int cpp_std)
        : preprocessor_bin{std::move(preprocessor_bin)}, smce_home{std::move(smce_home)}, lib_path{std::move(lib_path)}, cpp_std{cpp_std} {}

    ProgramOptions() = default;
};

ProgramOptions LoadProgramOptions(const stdfs::path& file);
void ExportProgramOptions(const ProgramOptions& config, const stdfs::path& path);

} // namespace smce

#endif // SMARTCAR_EMUL_PROGRAMOPTIONS_HXX
