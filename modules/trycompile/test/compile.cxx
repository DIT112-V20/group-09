#define CATCH_CONFIG_MAIN
#include <filesystem>
#include <catch2/catch.hpp>
#include "Toolchain.hxx"
#include "test_utility.hxx"

namespace stdfs = std::filesystem;

bool write_file(const stdfs::path& source_path, std::string_view src) {
    if (std::ofstream out(source_path, std::ios::out | std::ios::binary); out) {
        out.write(src.data(), src.size());
        return true;
    }
    return false;
}

bool test_compile(std::string_view src_name, std::string_view src) {
    if (!write_file(src_name, src))
        return false;
    const auto source_path = stdfs::absolute(stdfs::path(src_name));
    const auto ret = smce::compile_sketch(smce::SketchSource{source_path}, "../../");
    return !std::holds_alternative<std::runtime_error>(ret);
}

TEST_CASE("simple file compile", "[simple compile]") {
    constexpr auto test = R"(int main() {return 0;})";
    constexpr auto src_name = "simple.cpp";
    REQUIRE(test_compile(src_name, test));
}

TEST_CASE("C++17 file compile", "[C++17 compile]") {
    constexpr auto test = R"(#include <cstddef>
struct test { int a; float b; std::byte c; };
 int main() { const test tpl{}; auto& [a,b,c] = tpl; })";
    constexpr auto src_name = "C++17.cpp";
    REQUIRE(test_compile(src_name, test));
}

TEST_CASE("Non compliant file compile", "[Non compliant compile]") {
    constexpr auto test = R"(int main() { std::cout << "Hello world" << std::endl })";
    constexpr auto src_name = "error.cpp";
    REQUIRE(!test_compile(src_name, test));
}

TEST_CASE("simple sketch compile", "[simple compile]") {
    constexpr auto test = R"( void setup() {} void loop() {})";
    constexpr auto src_name = "sketch.ino";
    REQUIRE(test_compile(src_name, test));
}

TEST_CASE("simple sketch-pp compile", "[simple compile]") {
    constexpr auto test = R"( void setup() { f();
; } void loop() {} void f() {}
)";
    constexpr auto src_name = "sketch-pp.ino";
    REQUIRE(test_compile(src_name, test));
}