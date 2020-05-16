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

#include <condition_variable>
#include <mutex>
#include <thread>
#include "BoardData.hxx"
#include "Object.hxx"

namespace smce {

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
     * Resumes calling `loop()` if it was paused
     * Resumes board thread if interrupted
     * no-op otherwise
     **/
    bool resume() noexcept;
    /**
     * Do not execute `loop()` anymore if it running; no-op otherwise
     **/
    void pause_on_next_loop() noexcept;
    /**
     * Immediately suspends the board thread if running; no-op otherwise
     **/
    void pause_now() noexcept;
    /**
     * Tell the OS to terminate the thread followed by a call to clear(), if running or paused
     * no-op otherwise
     **/
    void murder() noexcept;

    bool set_sketch_and_car(SketchObject, BoardData&, const BoardInfo&) noexcept; // Should we really pass the data instead of the config?
    bool clear();

    [[nodiscard]] constexpr bool is_running() noexcept { return status == Status::running; }

    template <class Invocable, class... Args>
    void interrupt(Invocable&& invocable, Args&&... args) requires std::invocable<Invocable, decltype(args)...> {
        if (!is_running())
            return;
        pause_now();
        std::invoke(std::forward<Invocable>(invocable), std::forward<Args>(args)...);
        resume();
    }

  private:
    enum class Status : unsigned char {
        uninitialized,
        ready,
        running,
        suspended,
        loop_paused,
    };

    BoardData* vehicle_dat{};
    SketchLoadedObject curr_sketch;
    std::thread thr;       // warn: RAII
    bool stop_tok = false; // C++2a: make atomic
    bool exit_tok = false;
    std::condition_variable stop_cv{}; // C++2a: squash with above
    std::mutex stop_mut{};             // C++2a: squash with above
    Status status = Status::uninitialized;
};

} // namespace smce

#endif // SMARTCAR_EMUL_TRYCOMPILE_SKETCH_RUNTIME_HPP