#define CATCH_CONFIG_MAIN
#include <filesystem>
#include <iostream>
#include <string_view>
#include <catch2/catch.hpp>
#include "Toolchain.hxx"

bool simple_compile() {
    std::string t_compile = "#include <iostream>\nint main() {std::cout << \"Hello world\" << std::endl;}";
    auto src_name = "testing.cpp";
    auto source_path = std::filesystem::absolute(std::filesystem::path(src_name));
    std::ofstream test_src(source_path, std::ios::out | std::ios::binary);
    test_src.write(t_compile.c_str(), t_compile.size());
    test_src.close();
    auto ret = smce::compile_sketch(smce::SketchSource{source_path}, ".");
    return !std::holds_alternative<std::runtime_error>(ret);
}

TEST_CASE("simple file compile", "[simple compile]") { REQUIRE(simple_compile()); }
