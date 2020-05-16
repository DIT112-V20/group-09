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

#include <thread>
#include <utility>
#include "detail/Interrupt.hxx"
#include "BoardData.hxx"
#include "BoardInfo.hxx"
#include "Object.hxx"
#include "Runtime.hxx"

namespace smce {

SketchRuntime::~SketchRuntime() noexcept {
    switch (status) {
    case Status::uninitialized:
    case Status::ready:
        break;
    case Status::running:
    case Status::suspended:
        murder();
        clear();
        break;
    case Status::loop_paused:
        exit_tok = true;
        break;
    }
    if (thr.joinable())
        thr.join();
}

bool SketchRuntime::start() noexcept {
    if (status != Status::ready)
        return false;
    status = Status::running;
    thr = std::thread{[&]() {
        vehicle_dat->board_thread_id = std::this_thread::get_id();
        detail::make_self_suspendable(*(vehicle_dat->interrupt_mut = std::make_unique<std::recursive_mutex>()));
        curr_sketch.setup();
        while (true) {
            std::unique_lock lk{stop_mut};
            if (exit_tok)
                break;
            if (stop_tok) {
                stop_tok = false;
                stop_cv.wait(lk);
            }
            if (exit_tok)
                break;
            lk.unlock();
            curr_sketch.loop();
        }
    }};
    return true;
}

bool SketchRuntime::resume() noexcept {
    switch (status) {
    case Status::loop_paused:
        stop_tok = false;
        stop_cv.notify_all();
        break;
    case Status::suspended:
        detail::resume_thread(thr);
        break;
    default:
        return false;
    }
    status = Status::running;
    return true;
}

void SketchRuntime::pause_on_next_loop() noexcept {
    std::lock_guard lk{stop_mut};
    stop_tok = true;
    status = Status::loop_paused;
}

void SketchRuntime::pause_now() noexcept {
    if (status != Status::running)
        return;
    detail::suspend_thread(thr);
    status = Status::suspended;
}
void SketchRuntime::murder() noexcept {
    if (status != Status::running && status != Status::loop_paused && status != Status::suspended)
        return;
    detail::murder_thread(thr);
    clear();
}

bool SketchRuntime::set_sketch_and_car(SketchObject so, BoardData& bdata, const BoardInfo& binfo) noexcept {
    if (status != Status::uninitialized && status != Status::ready)
        return false;
    curr_sketch = load(std::move(so));
    if (!curr_sketch)
        return false;
    curr_sketch.init(&bdata, &binfo);
    vehicle_dat = &bdata;
    status = Status::ready;
    return true;
}

bool SketchRuntime::clear() {
    if (status != Status::uninitialized) {
        vehicle_dat = nullptr;
        curr_sketch = {};
        if (thr.joinable())
            thr.join();
        thr = {};
        status = Status::uninitialized;
        return true;
    }
    return false;
}

} // namespace smce