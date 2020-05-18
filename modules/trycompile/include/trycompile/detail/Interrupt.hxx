/*
 *  Interrupt.hxx
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

#ifndef SMARTCAR_EMUL_INTERRUPT_HXX
#define SMARTCAR_EMUL_INTERRUPT_HXX

#include <cstdint>
#include <functional>
#include <mutex>
#include <thread>

namespace smce::detail {

class SuspendableJThread {
    std::thread m_thread;
    std::recursive_mutex* m_mutex_ptr{};
    std::uint8_t m_started : 1 = false;
    std::uint8_t m_suspended : 1 = false;

    static void make_suspendable() noexcept;

  public:
    SuspendableJThread() noexcept = default;
    SuspendableJThread(const SuspendableJThread&) = delete;
    SuspendableJThread(SuspendableJThread&&) = delete;
    explicit SuspendableJThread(std::recursive_mutex* interrupt_mtx) noexcept : m_mutex_ptr{interrupt_mtx} {}
    ~SuspendableJThread() { if (m_thread.joinable()) m_thread.join(); }
    SuspendableJThread& operator=(const SuspendableJThread&) = delete;
    SuspendableJThread& operator=(SuspendableJThread&&) = delete;

    [[nodiscard]] auto get_id() const noexcept(noexcept(m_thread.get_id())) { return m_thread.get_id(); }
    [[nodiscard]] auto native_handle() noexcept(noexcept(m_thread.native_handle())) { return m_thread.native_handle(); }
    [[nodiscard]] bool joinable() const noexcept(noexcept(m_thread.joinable())) { return m_thread.joinable(); }
    void join() noexcept(noexcept(m_thread.join())) { return m_thread.join(); }
    void detach() noexcept(noexcept(m_thread.detach())) { return m_thread.detach(); }

    template <class Function, class... Args>
    void start(Function&& f, Args&&... args);

    /**
     * Sets what mutex to lock during suspension
     * \param mutex_ptr: pointer to the mutex, or nullptr to use no mutex
     * Note: thread unsafe
     **/
    void set_mutex(std::recursive_mutex* mutex_ptr) noexcept { m_mutex_ptr = mutex_ptr; }

    /**
     * Immediately suspends the board thread
     * No-op if called on a thread that either
     * - has not been started
     * - is already suspended and has no guarding mutex
     * - is murdered
     **/
    bool suspend() noexcept;
    bool resume() noexcept;
    bool murder() noexcept;

    void reset();
};

template <class Function, class... Args>
void SuspendableJThread::start(Function&& f, Args&&... args) {
    m_started = true;
    m_thread = std::thread([&, f=std::forward<Function>(f), ...args=std::forward<Args>(args)]() mutable {
       make_suspendable();
       std::invoke(std::forward<Function>(f), std::forward<Args>(args)...);
    });
}

inline void SuspendableJThread::reset() {
    this->~SuspendableJThread();
    new(this) SuspendableJThread;
}

} // namespace smce::detail

#endif // SMARTCAR_EMUL_INTERRUPT_HXX
