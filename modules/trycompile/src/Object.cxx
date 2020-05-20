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

#include "Object.hxx"

namespace smce {

SketchLoadedObject load(SketchObject so) {
    SketchLoadedObject ret;
    std::error_code ec;
    ret.dl.load(so.location, ec);
    if(ec)
        return {};


    if (ret.dl.has("init"))
        ret.init = ret.dl.get<SketchLoadedObject::InitType>("init");
    if (ret.dl.has("deinit"))
        ret.deinit = ret.dl.get<SketchLoadedObject::DeinitType>("deinit");
    if (ret.dl.has("setup"))
        ret.setup = ret.dl.get<SketchLoadedObject::SetupType>("setup");
    if (ret.dl.has("loop"))
        ret.loop = ret.dl.get<SketchLoadedObject::LoopType>("loop");
    return ret;
}

}