/*
 *  Toolchain.cxx
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

#include <array>
#include <string_view>
#include <fmt/format.h>
#include <boost/predef/os.h>
#include "Toolchain.hxx"
#include "utility.hxx"

using namespace std::literals;

namespace smce {

constexpr std::array type2str {"sketch"sv, "sktech_dir"sv, "sketch_dir_recurse"sv};

CompilationResults compile_sketch(SketchSource src, stdfs::path prefix, SourceType type) {
    if (!stdfs::exists(src.location))
        return CompilationResults{std::runtime_error{"Src location not found"}};

    const auto share_dir = prefix / "share/smce/toolchain";

    if (!stdfs::exists(share_dir))
        return CompilationResults{std::runtime_error{"Share location not found"}};

    const static auto tmp_dir = stdfs::temp_directory_path();
    const auto loc_hash = std::to_string(stdfs::hash_value(src.location));
    const auto build_dir = tmp_dir / loc_hash;

    if (stdfs::exists(build_dir))
        stdfs::remove_all(build_dir);
    stdfs::create_directory(build_dir);

#if BOOST_OS_UNIX
    constexpr std::string_view silent_output = "/dev/null";
#elif BOOST_OS_WINDOWS
    constexpr std::string_view silent_output = "NUL";
#else
    constexpr std::string_view silent_output = "null.txt"; // just in case
#endif


    const auto configure_cmd = fmt::format(R"(cmake -DSOURCE_FILE="{0}" -DSOURCE_TYPE="{5}" -DARDUINOCLI_PATH="{6}" -S "{1}" -B "{2}" && cmake --build "{4}")",
                                           src.location.string(), share_dir.string(), build_dir.string(), silent_output, build_dir.string(),
                                           type2str[static_cast<std::size_t>(type)], "");
    std::system(configure_cmd.c_str());

    auto obj_path = build_dir / loc_hash;
    return stdfs::exists((build_dir / loc_hash)) ? CompilationResults{SketchObject{std::move(obj_path)}}
                                                 : CompilationResults{std::runtime_error{"Compilation failed"}};
}

} // namespace smce
