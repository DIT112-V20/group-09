#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "Entrypoint.hxx"

TEST_CASE("Default-init", "[init]") {
    REQUIRE(init(nullptr, nullptr));
}