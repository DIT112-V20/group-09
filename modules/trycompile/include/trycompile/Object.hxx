/*
 *  Object.hxx
 *  Copyright 2020 AeroStun
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

#ifndef SMARTCAR_EMUL_TRYCOMPILE_SKETCH_OBJECT_HXX
#define SMARTCAR_EMUL_TRYCOMPILE_SKETCH_OBJECT_HXX

#include <string> // Workaroud Boost.DLL bug on MSVC19.25
#include <vector> // Workaroud Boost.DLL bug on macOS Clang10
#include <boost/dll.hpp>
#include <concepts>
#include <filesystem>
#include <type_traits>

struct BoardData;
struct BoardInfo;

namespace smce {

/**
 * Sketch shared object
 **/
struct SketchObject {
    std::filesystem::path location; /// Full filesystem path to the file
    std::filesystem::path runtime; /// Full filesystem path to the runtime file if any
};


/**
 *  Sketch shared object which is loaded
 **/
struct SketchLoadedObject {

    using InitType = bool(BoardData*, const BoardInfo*);
    using DeinitType = void();
    using SetupType = void();
    using LoopType = void();

    /// initializer function for the Arduino runtime
    InitType* init = nullptr;
    /// Deinitializer function for the Arduino runtime
    DeinitType* deinit = nullptr;
    /// Arduino setup function
    SetupType* setup = nullptr;
    /// Arduino loop function
    LoopType* loop = nullptr;

    SketchLoadedObject() noexcept = default;
    SketchLoadedObject(const SketchLoadedObject&) = delete;
    SketchLoadedObject(SketchLoadedObject&&) noexcept = default;
    ~SketchLoadedObject() noexcept = default;

    SketchLoadedObject& operator=(const SketchLoadedObject&) = delete;
    SketchLoadedObject& operator=(SketchLoadedObject&&) noexcept = default;

    /**
     * Checks if the object is valid
     * \return true if the object's contents can be used, false otherwise
     **/
    [[nodiscard]] inline explicit operator bool() const { return dl.is_loaded(); }

  private:
    boost::dll::shared_library dl;
    boost::dll::shared_library rt;

    friend SketchLoadedObject load(SketchObject);
    template <class R, class... Args>
    friend R invoke(const SketchLoadedObject&, const char* symbol, Args&&...);
};

/**
 *  Loads a sketch shared library object
 *  \param in input sketch
 *  \return valid loaded sketch, or invalid if failed to load
 **/
[[nodiscard]] SketchLoadedObject load(SketchObject in);

}

#include "impl/Object.txx"

#endif // SMARTCAR_EMUL_TRYCOMPILE_SKETCH_OBJECT_HXX