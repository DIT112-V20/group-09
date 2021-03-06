/*
 *  symbols.cxx
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

extern void setup();
extern void loop();

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__CYGWIN__)
#  define SYM_EXPORT __declspec(dllexport)
#  if defined(__GNUC__) || defined(__clang__)
#    undef SYM_EXPORT
#    define SYM_EXPORT __attribute__((__dllexport__))
#  endif
#else
#  define SYM_EXPORT __attribute__((__visibility__("default")))
#endif

struct BoardData;
struct BoardInfo;

extern bool init(BoardData*, const BoardInfo*);
extern void deinit();

extern "C" SYM_EXPORT bool SMCE__init(BoardData* pbd, const BoardInfo* pbi) { return init(pbd, pbi); }

extern "C" SYM_EXPORT void SMCE__deinit() { return deinit(); }

extern "C" SYM_EXPORT void SMCE__setup() { setup(); }

extern "C" SYM_EXPORT void SMCE__loop() { loop(); }