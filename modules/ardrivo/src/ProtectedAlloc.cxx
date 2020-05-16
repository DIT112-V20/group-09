/*
 *  ProtectedAlloc.cxx
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

#include <cstdlib>
#include <new>
#include <thread>
#include "BoardDataDef.hxx"

void* operator new(std::size_t len) {
    if (board_data && board_data->board_thread_id == std::this_thread::get_id()) {
        std::lock_guard lk{*board_data->interrupt_mut};
        return std::malloc(len);
    }
    return std::malloc(len);
}
void operator delete(void* ptr) noexcept {
    if (!ptr)
        return;
    if (board_data && board_data->board_thread_id == std::this_thread::get_id()) {
        std::lock_guard lk{*board_data->interrupt_mut};
        std::free(ptr);
        return;
    }
    std::free(ptr);
}
