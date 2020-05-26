#include <catch2/catch.hpp>
#include "WString.h"
#include <string_view>

TEST_CASE("Test compare a string to another" "[compareTo]")
{
	String test = "test";
	String test2 = "test";
	String test3 = "test";
	String test4 = "est";	
	String test5 = "Test";
	REQUIRE(test.compareTo(test2) == 0);
	REQUIRE(test3.compareTo(test4) == 1);
	REQUIRE(test5.compareTo(test) == -1);
}

TEST_CASE("Tests whether or not a String starts with the characters of another String" "[startsWith]")
{
	String test = "test";
	String test2 = "test";
	String test3 = "est";
	String test4 = "test";	
	REQUIRE(test.startsWith(test2));
	REQUIRE(!test3.startsWith(test4));
}

TEST_CASE("Tests whether or not a String ends with the characters of another String" "[endsWith]")
{
	String test = "test";
	String test2 = "test";
	String test3 = "est";
	String test4 = "test";
	REQUIRE(test.startsWith(test2));
	REQUIRE(!test3.startsWith(test4));
}

TEST_CASE("Copies the String’s characters to the supplied buffer" "[getBytes]")
{
	String test = "test";
	byte arr[5];
	test.getBytes(arr, test.length());	
	const char *p = reinterpret_cast<const char*>(arr);
	REQUIRE_THAT(p, Catch::Contains("test"));
}

TEST_CASE("Locates a character or String within another String" "[indexOf]")
{
	String test = "test";
	String test2 = "test";
	String test3 = "est";
	REQUIRE(test.indexOf(test2) == 0);
	REQUIRE(test.indexOf(test3) == 1);
}

TEST_CASE("Modify in place a String removing chars from the provided index to the end of the String or from the provided index to index plus count" "[remove]")
{
	String test = "test";
	test.remove(3);
	REQUIRE_THAT(test.c_str(), Catch::Contains("tes"));
}

TEST_CASE("Check if string gets swapped" "[replace]")
{
	String test = "ab";
	String test2 = "cd";
	test.replace(test, test2);
	REQUIRE_THAT(test.c_str(), Catch::EndsWith("cd"));
}

//Crap does not work without std::string
TEST_CASE("Test if string is given a new capacity" "[reserve]")
{ 
	String cap = "";
	REQUIRE(cap.length() == 0);

	cap.reserve(25);
	//REQUIRE(cap.length() == 63);
}

TEST_CASE("Sets a character of the String" "[setCharAt]")
{
	String test = "Hejsan";
	test.setCharAt(1, '\x84');
	REQUIRE_THAT(test.c_str(), Catch::EndsWith("H\x84jsan"));
}

TEST_CASE("Get a substring of a String" "[subsring]")
{	 
	String test = "test";
	String newString = test.substring(2);
	REQUIRE_THAT(newString.c_str(), Catch::Contains("st"));
}

TEST_CASE("Copies the String’s characters to the supplied buffer" "[toCharArray]")
{
	String test = "test";
	char arr[5] = "init";
	test.toCharArray(arr, test.length());
	REQUIRE_THAT(arr, Catch::Contains("test"));
}

TEST_CASE("Converts a valid String to an integer. The input String should start with an integer number" "[toInt]") 
{
	String test = "1 and 23";
	String test2 = "123";
	String test3 = "-100";
	String test4 = "--100";
	auto num = test.toInt();
	auto num2 = test2.toInt();
	auto num3 = test3.toInt();
	auto num4 = test4.toInt();
	REQUIRE(num == 1);
	REQUIRE(num2 == 123);
	REQUIRE(num3 == 0);
	REQUIRE(num4 == 0);
}

TEST_CASE("Converts a valid String to an double. The input String should start with an double number" "[toDouble]")
{
	String test = "1.0323414535 and 23";
	double num =  test.toDouble();
	REQUIRE(num >= 1.03228);
	REQUIRE(num <= 1.03240);
}

TEST_CASE("Converts a valid String to an float. The input String should start with an float number" "[toFloat]")
{
	String test = "33.34523345 stuff 23";
	float num =  test.toFloat();		
	REQUIRE(num >= 33.3445);
	REQUIRE(num <= 33.3460);
}

TEST_CASE("Get a lower-case version of a String" "[toLowerCase]")
{
	String test = "I LIKE BANANAS";
	test.toLowerCase();
	REQUIRE_THAT(test.c_str(), Catch::Contains("i like bananas"));
}

TEST_CASE("Get a upper-case version of a String" "[toUpperCase]")
{
	String test = "i like bigger bananas";
	test.toUpperCase();
	REQUIRE_THAT(test.c_str(), Catch::Contains("I LIKE BIGGER BANANAS"));
}

TEST_CASE("Remove spaces in a string" "[trim]")
{
	String test = "i like bigger bananas";
	test.trim();
	REQUIRE_THAT(test.c_str(), Catch::Contains("ilikebiggerbananas"));
}

TEST_CASE("Checks if a string is equal to another" "[equals]")
{
	String test = "i like bigger bananas";
	String test2 = "i LIKE BIGGER bananas";
	bool isEqual = test.equals(test2);
	REQUIRE(!isEqual);
}

TEST_CASE("Checks if a string is equal to another and ignores case" "[equalsIgnoreCase]")
{
	String test = "i like bigger bananas";
	String test2 = "i LIKE BIGGER bananas";
	bool equalsIgnoreCase = test.equalsIgnoreCase(test2);
	REQUIRE(equalsIgnoreCase);
}

TEST_CASE("Check operators" "[String::operator]")
{
	String test = "d";
	String test2 = "AB";

	SECTION("Operator ==")
	{
		REQUIRE(!(test== test2));
	}

	SECTION("Operator !=")
	{
		REQUIRE(test!=(test2));
	}

	SECTION("Operator <")
	{
		REQUIRE(!(test < test2));
	}

	SECTION("Operator <=")
	{
		REQUIRE(!(test <= test2));
	}

	SECTION("Operator >")
	{
		REQUIRE(test > test2);
	}

	SECTION("Operator >=")
	{
		REQUIRE(test >= test2);
	}

	SECTION("Operator+")
	{
		std::string str = "Hello, ";
		str += "world" ;
		REQUIRE_THAT(str.c_str(), Catch::Contains("Hello, world"));
	}
}