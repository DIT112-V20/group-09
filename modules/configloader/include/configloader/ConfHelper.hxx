/*
 *  ConfHelper.hxx
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

#ifndef SMARTCAR_EMUL_CONFHELPER_HXX
#define SMARTCAR_EMUL_CONFHELPER_HXX

#include <nameof.hpp>

template <class T, class C>
struct MptrKeyPair {
    using FieldType = T;
    using ClassType = C;

    T C::* mptr;
    const char* key;

    constexpr MptrKeyPair(T C::* mptr, const char* key) noexcept : mptr{mptr}, key{key} {}
};

template <class T>
concept MpKPair = std::is_same_v<T, MptrKeyPair<typename T::FieldType, typename  T::ClassType>>;

#define ADDR_AND_NAME(var) MptrKeyPair{&var, NAMEOF(var).c_str()}


#endif // SMARTCAR_EMUL_CONFHELPER_HXX
