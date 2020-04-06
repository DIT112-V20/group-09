/*
 *  Object.txx
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

#ifndef SMARTCAR_EMUL_TRYCOMPILE_SKETCH_OBJECT_TXX
#define SMARTCAR_EMUL_TRYCOMPILE_SKETCH_OBJECT_TXX

#include "Object.hxx"
#include <functional>
#include <string_view>
#include <system_error>

using namespace std::literals;

namespace smce {

template <class R, class... Args>
[[nodiscard]] R invoke(const SketchLoadedObject& obj, const char* symbol, Args&&... args) {
    if (!obj.dl.has(symbol))
        throw std::runtime_error{"Could not find requested symbol"};
    return std::invoke(obj.dl.get<F>(symbol, ec), std::forward<Args>(args)...);
}

}

#endif // SMARTCAR_EMUL_TRYCOMPILE_SKETCH_OBJECT_HXX