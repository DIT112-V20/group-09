#include <catch2/catch.hpp>
#include "WString.h"

using Catch::Contains;
using Catch::EndsWith; 

TEST_CASE("Test compare a string to another" "[compareTo]")
{
	String test = "test";
	String test2 = "test";
	String test3 = "test";
	String test4 = "est";	
	REQUIRE(!test.compareTo(test2));
	REQUIRE(test3.compareTo(test4));
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

TEST_CASE("Copies the String's characters to the supplied buffer" "[getBytes]")
{
	String test = "test";
	byte arr[5];
	test.getBytes(arr, test.length());	
	const char *p = reinterpret_cast<const char*>(arr);
	REQUIRE_THAT(p, Contains("test"));
}

TEST_CASE("Locates a character or String within another String" "[indexOf]")
{
	String test = "test";
	String test2 = "test";
	String test3 = "est";

	REQUIRE(!test.indexOf(test2));
	REQUIRE(test.indexOf(test3));
}

TEST_CASE("Modify in place a String removing chars from the provided index to the end of the String or from the provided index to index plus count" "[remove]")
{
	String test = "test";
	test.remove(3);
	char* test2 = new char[test.length() + 1];
	strcpy(test2, test.c_str());
	REQUIRE_THAT(test2, EndsWith( "tes" ));
}

TEST_CASE("Check if string gets swapped" "[replace]")
{
	String test = "ab";
	String test2 = "cd";
	test.replace(test, test2);
	char* arr = new char[test.length() + 1];
	strcpy(arr, test.c_str());	
	REQUIRE_THAT(arr, EndsWith( "cd" ));
}

TEST_CASE("Test if string is given a new capacity" "[reserve]")
{
	std::string test = "test is test";
	REQUIRE(test.capacity() == 15);
	test.reserve(50);
	REQUIRE(test.capacity() == 63);
}

TEST_CASE("Sets a character of the String" "[setCharAt]")
{
	String test = "Hejsan";
	test.setCharAt(1, '\x84');
	char* arr = new char[test.length() + 1];
	strcpy(arr, test.c_str());
	REQUIRE_THAT(arr, EndsWith("H\x84jsan"));
}

TEST_CASE("Get a substring of a String" "[subsring]")
{	
	String test = "test";
	String newString = test.substring(2);
	char* arr = new char[newString.length() + 1];
	strcpy(arr, newString.c_str());
	REQUIRE_THAT(arr, Contains("st"));
}

TEST_CASE("Copies the String’s characters to the supplied buffer" "[toCharArray]")
{
	String test = "test";
	char arr[5];
	test.toCharArray(arr, test.length());
	REQUIRE_THAT(arr, Contains("test"));
}

TEST_CASE("Converts a valid String to an integer. The input String should start with an integer number" "[toInt]") 
{
	String test = "1 and 23";
	unsigned long num =  test.toInt();
	REQUIRE(num == 1);
	String test2 = "123";
	unsigned long num2 =  test2.toInt();
	REQUIRE(num2 == 123);
}

TEST_CASE("Converts a valid String to an double. The input String should start with an double number" "[toDouble]")
{
	String test = "1.0323414535 and 23";
	double num =  test.toDouble();
	REQUIRE(num == 1.0323414535);
	String test2 = "123.4234512345";
	double num2 =  test2.toDouble();
	REQUIRE(num2 == 123.4234512345);
}

TEST_CASE("Converts a valid String to an float. The input String should start with an float number" "[toFloat]")
{
	String test = "1.0123 and 23";
	float num =  test.toFloat();
	REQUIRE(num == 1.0123f);
	String test2 = "123.23545";
	float num2 =  test2.toFloat();
	REQUIRE(num2 == 123.23545f);
}

TEST_CASE("Get a lower-case version of a String" "[toLowerCase]")
{
	String test = "I LIKE BANANAS";
	test.toLowerCase();
	char* arr = new char[test.length() + 1];
	strcpy(arr, test.c_str());
	REQUIRE_THAT(arr, Contains("i like bananas"));
}

TEST_CASE("Get a upper-case version of a String" "[toUpperCase]")
{
	String test = "i like bigger bananas";
	test.toUpperCase();
	char* arr = new char[test.length() + 1];
	strcpy(arr, test.c_str());
	REQUIRE_THAT(arr, Contains("I LIKE BIGGER BANANAS"));
}

TEST_CASE("Remove spaces in a string" "[trim]")
{
	String test = "i like bigger bananas";
	test.trim();
	char* arr = new char[test.length() + 1];
	strcpy(arr, test.c_str());
	REQUIRE_THAT(arr, Contains("ilikebiggerbananas"));
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
		REQUIRE(!(test==(test2)));
	}

	SECTION("Operator !=")
	{
		REQUIRE(test!=(test2));
	}

	SECTION("Operator <")
	{
		REQUIRE(!(test < (test2)));
	}

	SECTION("Operator <=")
	{
		REQUIRE(!(test <= (test2)));
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
		str.operator+=("world");
		char* arr = new char[str.length() + 1];
		strcpy(arr, str.c_str());
		REQUIRE_THAT(arr, Contains("Hello, world"));
	}
}