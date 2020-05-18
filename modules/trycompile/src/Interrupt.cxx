/*
 *  Interrupt.cxx
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

#include <cassert>
#include <ciso646>
#include <condition_variable>
#include <csignal>
#include <mutex>
#include <thread>
#include <type_traits>

/* Platform detection */
#if defined(__GLIBCPP__) || defined(__GLIBCXX__) // GNU libstdc++
#   define SMCE_STDLIB_LIBSTDCXX 1
#   ifdef SMCE_THREADS_MINGW_WIN32
#       define SMCE_THREADS_WIN32 1
#   elif defined(_GLIBCXX__PTHREADS) || defined(_GLIBCXX_GCC_GTHR_POSIX_H)
#       define SMCE_THREADS_POSIX 1
#   else
#       pragma GCC warning "Could not detect underlying thread implementation"
#   endif
#elif defined(_LIBCPP_VERSION) // LLVM libc++
#   define SMCE_STDLIB_LIBCXX 1
#   if defined(_LIBCPP_HAS_NO_THREADS)
#       error "Detected LLVM libc++ without thread support"
#   elif defined(_LIBCPP_HAS_THREAD_API_PTHREAD)
#       define SMCE_THREADS_POSIX 1
#   elif defined(_LIBCPP_HAS_THREAD_API_WIN32)
#       define SMCE_THREADS_WIN32
#   endif
#elif defined(_YVALS) // Microsoft STL or Dinkumware
#   define SMCE_STDLIB_MSSTL_OR_DINKUMWARE
#   define SMCE_THREADS_WIN32 1
#else
#   error "Unknown/unsupported C++ standard library"
#endif


/* Platform-specific headers */
#ifdef SMCE_THREADS_POSIX
#if !__has_include(<pthread.h>)
#error "PThreads chosen but header not available"
#endif
#include <pthread.h>
#include <unistd.h>
#endif
#ifdef SMCE_THREADS_WIN32
#include <Windows.h>
#endif


/* Implementations */
#include <detail/Interrupt.hxx>

namespace smce::detail {

#ifdef SMCE_THREADS_POSIX
#if defined(_POSIX_C_SOURCE) || defined(_XOPEN_SOURCE) || defined(__APPLE__)
#if _POSIX_C_SOURCE >= 199506L || _XOPEN_SOURCE >= 500 || defined(__APPLE__)

static_assert(std::is_same_v<std::thread::native_handle_type, pthread_t>);

std::condition_variable g_signal_awaiter_cv;
std::mutex g_signal_awaiter_guard;

void SuspendableJThread::make_suspendable() noexcept {
    // For termination
    int unused;
    assert(pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &unused) == 0);
    assert(pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &unused) == 0);

    // For suspension
    struct sigaction sa;
    sa.sa_flags = 0;
    sa.sa_handler = +[](int sig) noexcept -> void {
        switch (sig) {
        case SIGUSR1:
            g_signal_awaiter_cv.notify_all();
            pause();
            break;
        case SIGUSR2:
            break;
        case SIGTERM:
            pthread_exit(nullptr);
        }
    };

    assert(sigemptyset(&sa.sa_mask) == 0);
    assert(sigaction(SIGUSR1, &sa, nullptr) == 0);
    assert(sigaction(SIGUSR2, &sa, nullptr) == 0);
    assert(sigaction(SIGTERM, &sa, nullptr) == 0);
}

bool SuspendableJThread::suspend() noexcept {
    if (!m_started || (m_suspended && !m_mutex_ptr))
        return false;
    if (m_mutex_ptr)
        m_mutex_ptr->lock();
    std::lock_guard lk{g_signal_awaiter_guard};
    std::mutex interrupt_wait_mutex;
    std::unique_lock ulk{interrupt_wait_mutex};
    if (pthread_kill(m_thread.native_handle(), SIGUSR1) != 0) {
        if (m_mutex_ptr)
            m_mutex_ptr->unlock();
        return false;
    }
    g_signal_awaiter_cv.wait(ulk);
    m_suspended = true;
    return true;
}

bool SuspendableJThread::resume() noexcept {
    if (!m_started)
        return false;
    if (pthread_kill(m_thread.native_handle(), SIGUSR2) != 0)
        return false;
    if (m_mutex_ptr)
        m_mutex_ptr->unlock();
    m_suspended = false;
    return true;
}

bool SuspendableJThread::murder() noexcept {
    if (!m_started)
        return true;
    const pthread_t hdl = m_thread.native_handle();
    if (pthread_cancel(hdl) != 0)
        return false;
    void* res{};
    if (pthread_join(hdl, &res) != 0)
        return false;
    const bool ret = (res == PTHREAD_CANCELED);
    if (ret) {
        m_started = false;
        m_suspended = false;
    }
    return ret;
}

#else
#error "PThreads chosen but feature test macros have outdated versions"
#endif
#else
#error "PThreads chosen but feature test macros not defined"
#endif
#endif

#ifdef SMCE_THREADS_WIN32
#if defined(_MSC_VER)

void SuspendableJThread::make_suspendable() noexcept {}

bool SuspendableJThread::suspend() noexcept {
    if (!m_started || (m_suspended && !m_mutex_ptr))
        return false;
    if (m_mutex_ptr)
        m_mutex_ptr->lock();
    if (SuspendThread(m_thread.native_handle()) == -1) {
        if (m_mutex_ptr)
            m_mutex_ptr->unlock();
        return false;
    }
    m_suspended = true;
    return true;
}

bool SuspendableJThread::resume() noexcept {
    if (!m_started)
        return false;
    if (ResumeThread(m_thread.native_handle()) == -1)
        return false;
    if (m_mutex_ptr)
        m_mutex_ptr->unlock();
    m_suspended = false;
    return true;
}

bool SuspendableJThread::murder() noexcept {
    const bool ret = TerminateThread(m_thread.native_handle(), -1);
    if (ret) {
        m_started = false;
        m_suspended = false;
    }
    return ret;
}

#endif
#endif

} // namespace smce::detail