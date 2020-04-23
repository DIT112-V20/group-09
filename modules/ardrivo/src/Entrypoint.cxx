/*
 *  Entrypoint.cxx
 *  Copyright 2020 AeroStun
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

#include "BoardData.hxx"
#include "BoardInfo.hxx"
#include "Entrypoint.hxx"

BoardData* board_data;
const BoardInfo* board_info;

bool init(BoardData* brd, const BoardInfo* info) {
    board_data = brd;
    board_info = info;
    return true;
}