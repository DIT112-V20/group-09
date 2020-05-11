
#include <BoardDataDef.hxx>
#include <catch2/catch.hpp>
#include <range/v3/algorithm/for_each.hpp>
#include "Arduino.h"
#include "BoardData.hxx"
#include "BoardInfo.hxx"
#include "Entrypoint.hxx"


static void init_fake()
{
	if (board_data)
		return;

	auto loc_board_data = std::make_unique<BoardData>();
	loc_board_data->silence_errors = false;
}

TEST_CASE("Test HardwareSerial begin", "[begin]")
{

}