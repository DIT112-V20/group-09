/*
 *  Object.cpp
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

#include <iostream>
#include "Object.hxx"

constexpr auto init = "init";
constexpr auto deinit = "deinit";
constexpr auto setup = "setup";
constexpr auto loop = "loop";

namespace smce {

SketchLoadedObject load(SketchObject so) {
    SketchLoadedObject ret;
    std::error_code ec;
    using namespace boost::dll::load_mode;

    if(!so.runtime.empty()) {
        ret.rt.load(so.runtime, ec, append_decorations);
        if (ec) {
            std::cerr << ec.message() << std::endl;
            return {};
        }
    }

    ret.dl.load(so.location, ec, rtld_now);
    if (ec) {
        std::cerr << ec.message() << std::endl;
        return {};
    }

    auto load_or_override = [&]<class F>(const auto& sym, F*(SketchLoadedObject::* field)) {
      if (ret.dl.has(sym))
          (ret.*field) = ret.dl.get<F>(sym);
      else if (ret.rt.has(sym))
          (ret.*field) = ret.rt.get<F>(sym);
      else {
          std::cerr << "Required symbol " << sym << " not found in sketch binary or runtime" << std::endl;
          return false;
      }
      return true;
    };

    assert(load_or_override(init, &SketchLoadedObject::init));
    assert(load_or_override(deinit, &SketchLoadedObject::deinit));
    if (ret.dl.has(setup))
        ret.setup = ret.dl.get<SketchLoadedObject::SetupType>(setup);
    else
        std::cerr << "Required symbol " << setup << " not found in sketch binary" << std::endl;
    if (ret.dl.has(loop))
        ret.loop = ret.dl.get<SketchLoadedObject::LoopType>(loop);
    else
        std::cerr << "Required symbol " << loop << " not found in sketch binary" << std::endl;
    return ret;
}

}