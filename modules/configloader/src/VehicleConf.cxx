/*
 *  VehicleConf.cxx
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

#include <fstream>
#include <optional>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wambiguous-reversed-operator"
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#pragma clang diagnostic pop
#include <range/v3/algorithm/transform.hpp>
#include "VehicleConf.hxx"

auto smce::VehicleConfig::load(rapidjson::Document doc) -> std::optional<VehicleConfig> {
    std::optional<VehicleConfig> ret;
    auto& conf = ret.emplace();

    conf.m_doc = std::move(doc);

    if(!conf.m_doc.HasMember("hull_model_file"))
        return std::nullopt;
    if(!conf.m_doc["hull_model_file"].IsString())
        return std::nullopt;
    conf.hull_model_file = doc["hull_model_file"].GetString();

    if(!conf.m_doc.HasMember("wheel_model_file"))
        return std::nullopt;
    if(!conf.m_doc["wheel_model_file"].IsString())
        return std::nullopt;
    conf.wheel_model_file = doc["wheel_model_file"].GetString();

    if(!conf.m_doc.HasMember("attachments"))
        return std::nullopt;
    if(!conf.m_doc["attachments"].IsArray())
        return std::nullopt;
    auto j_attach = conf.m_doc["attachements"].GetObject();
    for(const auto& el : j_attach){
        auto& em = conf.attachments.emplace_back();
        em.first = el.name.GetString();
        em.second = &el.value;
    }

    return ret;
}

auto smce::VehicleConfig::load(const stdfs::path& file_location) -> std::optional<VehicleConfig> {
    rapidjson::Document doc;
    std::ifstream ifs{file_location};
    if(!ifs)
        [[unlikely]] return std::nullopt;

    rapidjson::IStreamWrapper isw(ifs);
    if(doc.ParseStream(isw).HasParseError())
        [[unlikely]] return std::nullopt;

    return load(std::move(doc));
}