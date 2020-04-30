/*
 *  VehicleConf.hxx
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

#ifndef SMARTCAR_EMUL_VEHICLECONF_HXX
#define SMARTCAR_EMUL_VEHICLECONF_HXX

#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <rapidjson/document.h>

namespace smce {

namespace stdfs = std::filesystem;

class VehicleConfig {
    rapidjson::Document m_doc;
  public:
    std::filesystem::path hull_model_file;
    std::filesystem::path wheel_model_file;
    std::vector<std::pair<std::string, const rapidjson::Value*>> attachments;

    /**
     * Loads a vehicle config from a JSON document
     * \param doc containing the config
     * \return the vehicle config on success, std::nullopt otherwise
     * */
    static std::optional<VehicleConfig> load(rapidjson::Document doc);
    /**
     * Loads a vehicle config from a JSON file
     * \param file_path of the file containing
     * \return the vehicle config on success, std::nullopt otherwise
     * */
    static std::optional<VehicleConfig> load(const stdfs::path& file_path);
};

}

#endif // SMARTCAR_EMUL_VEHICLECONF_HXX
