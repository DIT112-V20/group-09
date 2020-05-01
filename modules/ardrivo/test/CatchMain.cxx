#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>
#include "BoardDataDef.hxx"

int main(int argc, char** argv) {
    int result = Catch::Session().run(argc, argv);
    if(board_data)
        (delete board_data), board_data = nullptr;
    if(board_info)
        (delete board_info), board_data = nullptr;

    return result;
}