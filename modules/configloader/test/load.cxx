#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "Loader.hxx"

namespace stdfs = std::filesystem;

bool write_file(const stdfs::path& source_path, std::string_view src) {
    if (std::ofstream out(source_path, std::ios::out | std::ios::binary); out) {
        out.write(src.data(), src.size());
        return true;
    }
    return false;
}

TEST_CASE("Load config file", "[load config]") {
    constexpr auto ini = R"([paths]
preprocessor_bin=testing
smce_home=testing
lib_path=testing
[versions]
cpp_std=14
)";
    constexpr auto ini_name = "sample.ini";

    std::error_code e;
    std::filesystem::create_directory("testing", e);
    REQUIRE(!e);

    REQUIRE(write_file(ini_name, ini));
    auto config = smce::LoadProgramOptions(ini_name);

    REQUIRE(config.preprocessor_bin == "testing");
    REQUIRE(config.smce_home == "testing");
    REQUIRE(config.lib_path == "testing");
    REQUIRE(config.cpp_std == 14);
}

TEST_CASE("Non existent directories", "[load bad dirs config]") {
    constexpr auto ini = R"([paths]
preprocessor_bin=nottesting
smce_home=nottesting
lib_path=nottesting
[versions]
cpp_std=17
)";
    constexpr auto ini_name = "sample_bad.ini";

    REQUIRE(write_file(ini_name, ini));
    auto config = smce::LoadProgramOptions(ini_name);

    REQUIRE(config.preprocessor_bin == "");
    REQUIRE(config.smce_home == ".");
    REQUIRE(config.lib_path == ".");
    REQUIRE(config.cpp_std == 17);
}

TEST_CASE("Parsing fail config", "[Parse fail]") {
    constexpr auto ini = R"(ths]
preprocessor_bin=nottesting
lib_path=nottesting
[vers
cpp_std17
)";
    constexpr auto ini_name = "sample_fail.ini";

    REQUIRE(write_file(ini_name, ini));
    auto config = smce::LoadProgramOptions(ini_name);

    REQUIRE(config.preprocessor_bin == "");
    REQUIRE(config.smce_home == ".");
    REQUIRE(config.lib_path == ".");
    REQUIRE(config.cpp_std == 17);
}

TEST_CASE("Export config", "[Export config]") {
    std::error_code e;
    std::filesystem::create_directory("helloworld", e);
    REQUIRE(!e);

    auto test = smce::ProgramOptions{};
    test.cpp_std = 11;
    test.smce_home = "helloworld";

    const auto out = stdfs::path{"export.ini"};
    smce::ExportProgramOptions(test, out);
    const auto comp = smce::LoadProgramOptions(out);

    REQUIRE(test.preprocessor_bin == comp.preprocessor_bin);
    REQUIRE(test.smce_home == comp.smce_home);
    REQUIRE(test.lib_path == comp.lib_path);
    REQUIRE(test.cpp_std == comp.cpp_std);
}
