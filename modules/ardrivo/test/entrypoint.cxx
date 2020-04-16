#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "entrypoint.hxx"

TEST_CASE("Default-init", "[init]") {
    REQUIRE(init(nullptr));
}