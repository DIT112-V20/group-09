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
#include <boost/predef/os.h>

namespace smce {
namespace stdfs = std::filesystem;

#if BOOST_OS_UNIX || BOOST_OS_MACOS
const stdfs::path PLATFORM_SMCE_HOME = "/usr/share/smce";
#elif BOOST_OS_WINDOWS
const stdfs::path PLATFORM_SMCE_HOME = std::filesystem::current_path().root_path() / "Program Files" / "Smce";
#else
const stdfs::path PLATFORM_SMCE_HOME = ".";
#endif

struct ProgramOptions {

    static constexpr auto DEFAULT_CONFIG_FILE_NAME = "smce_options.ini";
    static constexpr auto DEFAULT_ARDUINOCLI_BIN = "";
    static constexpr auto DEFAULT_SMCE_HOME = ".";
    static constexpr auto DEFAULT_VEHICLE_CONFIG = "";
    static constexpr auto DEFAULT_BOARD_CONFIG = "";
    static constexpr auto DEFAULT_LIB_PATH = ".";

    stdfs::path arduinocli_bin = DEFAULT_ARDUINOCLI_BIN;
    stdfs::path smce_home = DEFAULT_SMCE_HOME;
    stdfs::path vehicle_config = DEFAULT_VEHICLE_CONFIG;
    stdfs::path board_config = DEFAULT_BOARD_CONFIG;

    stdfs::path lib_path = DEFAULT_LIB_PATH;

    // TODO: Remove when P0960R3 is supported
    ProgramOptions(stdfs::path preprocessor_bin, stdfs::path smce_home, stdfs::path vehicle_config, stdfs::path board_config, stdfs::path lib_path)
        : arduinocli_bin{std::move(preprocessor_bin)}, smce_home{std::move(smce_home)}, vehicle_config{std::move(vehicle_config)}, board_config{std::move(board_config)}, lib_path{std::move(lib_path)} {}

    ProgramOptions() = default;
};

ProgramOptions LoadProgramOptions(const stdfs::path& file);
void ExportProgramOptions(const ProgramOptions& config, const stdfs::path& path);

} // namespace smce

#endif // SMARTCAR_EMUL_PROGRAMOPTIONS_HXX
