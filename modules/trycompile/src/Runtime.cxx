/*
 *  Runtime.cxx
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

#include "Runtime.hxx"
#include "Object.hxx"
#include <iostream>
#include <utility>

namespace {

void looper(const smce::SketchLoadedObject& slo, std::atomic_bool& stop_tok, bool& running) noexcept {
    running = true;
    while (!stop_tok)
        slo.loop();
    stop_tok = running = false;
}

}

namespace smce {

SketchRuntime::~SketchRuntime() noexcept {
    pause_on_next_loop();
    if (thr.joinable())
        thr.join();
}

bool SketchRuntime::start() noexcept {
    if (running)
        return false;
    curr_sketch.setup();
    if (thr.joinable())
        thr.join();
    launch_thread_unchecked();
    return true;
}

bool SketchRuntime::resume() noexcept {
    if (running)
        return false;
    if (thr.joinable())
        thr.join();
    launch_thread_unchecked();
    return true;
}

void SketchRuntime::pause_on_next_loop() noexcept { stop_tok = true; }

bool SketchRuntime::set_sketch_and_car(SketchObject so, VehicleData& vdata) noexcept {
    if (running)
        return false;
    curr_sketch = load(std::move(so));
    if (!curr_sketch)
        return false;
    curr_sketch.init(
        &vdata, +[](unsigned char c) -> bool {
          return static_cast<bool>(std::cout.put(c));
        },
        +[](const unsigned char* buf, std::size_t len) -> std::size_t {
          std::cout.write(reinterpret_cast<const char*>(buf), len);
          return std::cout ? len : 0;
        });
    vehicle_dat = &vdata;
    return true;
}

bool SketchRuntime::clear() {
    if (running) {
        vehicle_dat = nullptr;
        curr_sketch = {};
        thr = {};
        running = false;
    }
    return !running;
}

void SketchRuntime::launch_thread_unchecked() {
    thr = std::thread{looper, std::cref(curr_sketch), std::ref(stop_tok), std::ref(running)};
}

}