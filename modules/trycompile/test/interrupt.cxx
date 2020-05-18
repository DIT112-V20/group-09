#include <chrono>
#include <filesystem>
#include <thread>
#include <variant>
#include <catch2/catch.hpp>
#include <utility.hxx>
#include "BoardInfo.hxx"
#include "Runtime.hxx"
#include "Toolchain.hxx"
#include "test_utility.hxx"

using namespace std::literals;

namespace stdfs = std::filesystem;
TEST_CASE("Interruptability", "[Arduino compile]") {
constexpr std::string_view src = R"(
void setup() {}
void loop() { pinMode(0, OUTPUT); })";
constexpr std::string_view src_name = "arduino_interruptable.ino";

REQUIRE(write_file(src_name, src));
const auto source_path = stdfs::absolute(stdfs::path(src_name));
auto ret = smce::compile_sketch(smce::SketchSource{source_path}, "../../");
std::visit(Visitor{[](smce::SketchObject so) {
        auto ld = smce::load(so);
        REQUIRE(ld);
        REQUIRE(ld.init);
        REQUIRE(ld.loop);
        REQUIRE(ld.setup);
        BoardData bd;
        bd.pin_modes = std::vector<std::atomic_uint8_t>(1);
        BoardInfo bi;
        bi.pins_caps.resize(1);
        bi.pins_caps[0].digital_in = true;
        bi.pins_caps[0].digital_out = true;
        smce::SketchRuntime sr;
        sr.set_sketch_and_car(std::move(so), bd, bi);
        sr.start();
        std::this_thread::sleep_for(2s); // Let the thread scheduler do its thing
        REQUIRE(bd.pin_modes[0] == '\x01');
        sr.interrupt([&](){
            bd.pin_modes[0] = 255u;
            std::this_thread::sleep_for(2s); // Let the thread scheduler do its thing;
            REQUIRE(bd.pin_modes[0] == 255);
        });
        std::this_thread::sleep_for(2s); // Let the thread scheduler do its thing;
        REQUIRE(bd.pin_modes[0] == '\x01');
        sr.pause_on_next_loop();
        std::this_thread::yield();
    },
    [](const std::runtime_error& err) { REQUIRE(false); }},
  std::move(ret));
}