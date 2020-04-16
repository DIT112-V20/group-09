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
#include "Object.hxx"
#include "Source.hxx"

namespace smce {
namespace stdfs = std::filesystem;

enum struct SourceType {
    sketch,
    sktech_dir,
    sketch_dir_recurse,
};

using CompilationResults = std::variant<SketchObject, std::runtime_error>;
/**
 * Attempts to compile a sketch file
 * \param src The source file/project to compile
 * \param location SMCE_HOME
 * \param type What the sketch consists of
 * \return the results of the compilation (see CompilationResults)
 **/
CompilationResults compile_sketch(SketchSource src, stdfs::path location, SourceType type = SourceType::sketch);

[[nodiscard]] inline auto launch_compile(SketchSource src, stdfs::path location, SourceType type) { return std::async(std::launch::async, compile_sketch, std::move(src), std::move(location), type); }

} // namespace smce

#endif // SMARTCAR_EMUL_TRYCOMPILE_SKETCH_TOOLCHAIN_HXX