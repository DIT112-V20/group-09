/*
 *  UrhoUtility.hxx
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
 */

#ifndef SMARTCAR_EMUL_URHOUTILITY_HXX
#define SMARTCAR_EMUL_URHOUTILITY_HXX
#include <utility>
#include <Urho3D/Container/Ptr.h>

namespace Urho3D {
template <class T, class... Args>
Urho3D::SharedPtr<T> MakeShared(Args&&... args) {
    return Urho3D::SharedPtr<T>(new T(std::forward<Args>(args)...));
}
template <class T, class... Args>
Urho3D::UniquePtr<T> MakeUnique(Args&&... args) {
    return Urho3D::SharedPtr<T>(new T(std::forward<Args>(args)...));
}
}
#endif // SMARTCAR_EMUL_URHOUTILITY_HXX
