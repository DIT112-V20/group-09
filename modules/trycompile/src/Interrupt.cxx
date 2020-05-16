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

#include <ciso646>
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

thread_local std::recursive_mutex* gtl_interrupt_mutex{};

#ifdef SMCE_THREADS_POSIX
#if defined(_POSIX_C_SOURCE) || defined(_XOPEN_SOURCE) || defined(__APPLE__)
#if _POSIX_C_SOURCE >= 199506L || _XOPEN_SOURCE >= 500 || defined(__APPLE__)

static_assert(std::is_same_v<std::thread::native_handle_type, pthread_t>);

bool make_self_suspendable(std::recursive_mutex& guard) noexcept {
    // For termination
    int unused;
    if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &unused) != 0)
        return false;
    if (pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &unused) != 0)
        return false;

    // For suspension
    struct sigaction sa;
    sa.sa_flags = 0;
    sa.sa_handler = +[](int sig) noexcept -> void {
        switch (sig) {
        case SIGUSR1:
            pause();
            break;
        case SIGUSR2:
            break;
        case SIGTERM:
            pthread_exit(nullptr);
        }
    };

    if (sigemptyset(&sa.sa_mask) != 0)
        return false;
    if (sigaction(SIGUSR1, &sa, nullptr) == -1)
        return false;
    if (sigaction(SIGUSR2, &sa, nullptr) == -1)
        return false;
    if (sigaction(SIGTERM, &sa, nullptr) == -1)
        return false;

    gtl_interrupt_mutex = &guard;
    return true;
}

bool suspend_thread(std::thread& th) noexcept {
    if (!gtl_interrupt_mutex)
        return false;
    gtl_interrupt_mutex->lock();
    const auto res = pthread_kill(th.native_handle(), SIGUSR1);
    if (res != 0)
        gtl_interrupt_mutex->unlock();
    return res == 0;
}

bool resume_thread(std::thread& th) noexcept {
    if (!gtl_interrupt_mutex)
        return false;
    const auto res = pthread_kill(th.native_handle(), SIGUSR2);
    if (res != 0)
        return false;
    gtl_interrupt_mutex->unlock();
    return true;
}

bool murder_thread(std::thread& th) noexcept {
    const pthread_t hdl = th.native_handle();
    if (pthread_cancel(hdl) != 0)
        return false;
    void* res{};
    if (pthread_join(hdl, &res) != 0)
        return false;
    return res == PTHREAD_CANCELED;
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

bool make_self_suspendable(std::recursive_mutex& guard) noexcept {
    gtl_interrupt_mutex = &guard;
    return true;
}

bool suspend_thread(std::thread& th) noexcept {
    if (!gtl_interrupt_mutex)
        return false;
    gtl_interrupt_mutex->lock();
    const auto res = SuspendThread(th.native_handle());
    if (res == -1)
        gtl_interrupt_mutex->unlock();
    return res != -1;
}

bool resume_thread(std::thread& th) noexcept {
    if (!gtl_interrupt_mutex)
        return false;
    const auto res = ResumeThread(th.native_handle());
    if (res != 0)
        return false;
    gtl_interrupt_mutex->unlock();
    return true;
}

bool murder_thread(std::thread& th) noexcept {
    return TerminateThread(th.native_handle(), -1);
}

#endif
#endif

} // namespace smce::detail