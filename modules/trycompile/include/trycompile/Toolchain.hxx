/*
 *  Toolchain.hxx
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

#ifndef SMARTCAR_EMUL_TRYCOMPILE_SKETCH_TOOLCHAIN_HXX
#define SMARTCAR_EMUL_TRYCOMPILE_SKETCH_TOOLCHAIN_HXX

#include <exception>
#include <filesystem>
#include <future>
#include <variant>
#include "boost/predef/os.h"
#include "Object.hxx"
#include "Source.hxx"

namespace smce {
namespace stdfs = std::filesystem;

#ifdef BOOST_PREDEF_OS_UNIX_H
constexpr std::string_view silent_output = "/dev/null";
#elif BOOST_PREDEF_OS_WINDOWS_H
constexpr std::string_view silent_output = "NUL";
#else
constexpr std::string_view silent_output = "null.txt"; // just in case
#endif

using CompilationResults = std::variant<SketchObject, std::runtime_error>;
/**
 * Attempts to compile a sketch file
 * \param src The source file to compile
 * \param location SMCE_HOME
 * \return the results of the compilation (see CompilationResults)
 **/
CompilationResults compile_sketch(SketchSource src, stdfs::path location);

[[nodiscard]] inline auto launch_compile(SketchSource src, stdfs::path location) { return std::async(std::launch::async, compile_sketch, std::move(src), std::move(location)); }

} // namespace smce

#endif // SMARTCAR_EMUL_TRYCOMPILE_SKETCH_TOOLCHAIN_HXX