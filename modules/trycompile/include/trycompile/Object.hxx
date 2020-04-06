/*
 *  Object.hpp
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

#include <boost/dll.hpp>
#include <filesystem>

namespace smce {

struct SketchObject {
    std::filesystem::path location;
};

class VehicleData;

struct SketchLoadedObject {
    using WriteByte = bool(*)(unsigned char);
    using WriteBuf = std::size_t (*)(const unsigned char*, size_t);

    using InitType = bool(VehicleData*, WriteByte, WriteBuf);
    using SetupType = void();
    using LoopType = void();

    InitType* init = nullptr;
    SetupType* setup = nullptr;
    LoopType* loop = nullptr;

    SketchLoadedObject() noexcept = default;
    SketchLoadedObject(const SketchLoadedObject&) = delete;
    SketchLoadedObject(SketchLoadedObject&&) noexcept = default;
    ~SketchLoadedObject() noexcept = default;

    SketchLoadedObject& operator=(const SketchLoadedObject&) = delete;
    SketchLoadedObject& operator=(SketchLoadedObject&&) noexcept = default;

    [[nodiscard]] inline explicit operator bool() const { return dl.is_loaded(); }

  private:
    boost::dll::shared_library dl;

    friend SketchLoadedObject load(SketchObject);
};

[[nodiscard]] SketchLoadedObject load(SketchObject);

}

#endif // SMARTCAR_EMUL_TRYCOMPILE_SKETCH_OBJECT_HXX