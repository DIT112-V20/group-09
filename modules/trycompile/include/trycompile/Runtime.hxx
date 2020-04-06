/*
 *  Runtime.hxx
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

#ifndef SMARTCAR_EMUL_TRYCOMPILE_SKETCH_RUNTIME_HPP
#define SMARTCAR_EMUL_TRYCOMPILE_SKETCH_RUNTIME_HPP

#include "Object.hxx"
#include <thread>

namespace smce {

class VehicleData;

/**
 * Runtime in which a sktech is to be executed
 **/
struct SketchRuntime {
    SketchRuntime() = default; // FIXME: do we need it?
    SketchRuntime(const SketchRuntime&) = delete;
    SketchRuntime(SketchRuntime&&) = delete;
    ~SketchRuntime() noexcept;
    SketchRuntime& operator=(const SketchRuntime&) = delete;
    SketchRuntime& operator=(SketchRuntime&&) = delete;


    /**
     * Runs the sketch's `setup()` function if sketch is loaded
     **/
    bool start() noexcept;
    /**
     * Resumes calling `loop()` if it was stalled; no-op otherwise
     **/
    bool resume() noexcept;
    /**
     * Do not execute `loop()` anymore if it running; no-op otherwise
     **/
    void pause_on_next_loop() noexcept;


    bool set_sketch_and_car(SketchObject, VehicleData&) noexcept; // Should we really pass the data instead of the config?
    bool clear();

    [[nodiscard]] constexpr bool is_running() noexcept { return running; }

  private:
    void launch_thread_unchecked();

    VehicleData* vehicle_dat{}; // This will probably end up a value member
    SketchLoadedObject curr_sketch;
    std::thread thr; // warn: RAII
    std::atomic_bool stop_tok{};
    bool running = false;
};

}

#endif // SMARTCAR_EMUL_TRYCOMPILE_SKETCH_RUNTIME_HPP