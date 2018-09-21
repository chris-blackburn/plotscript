#include "catch.hpp"

#include "expression.hpp"

TEST_CASE("Test default expression", "[expression]") {
	Expression exp;

	REQUIRE(!exp.isHeadNumber());
	REQUIRE(!exp.isHeadComplex());
	REQUIRE(!exp.isHeadSymbol());
}

TEST_CASE("Test expression list constructor", "[expression]") {
	std::vector<Expression> list = {Expression(1), Expression(complex(0, 1)),
		Expression(Atom("hi"))};
	Expression exp(list);

	// The head atom is of type NoneKind - it acts like a place holder
	REQUIRE(!exp.isHeadNumber());
	REQUIRE(!exp.isHeadComplex());
	REQUIRE(!exp.isHeadSymbol());
}

TEST_CASE("Test double expression", "[expression]") {
	Expression exp(6.023);

	REQUIRE(exp.isHeadNumber());
	REQUIRE(!exp.isHeadComplex());
	REQUIRE(!exp.isHeadSymbol());
}

TEST_CASE("Test complex expression", "[expression]") {
	Expression exp(complex(1, 1));

	REQUIRE(!exp.isHeadNumber());
	REQUIRE(exp.isHeadComplex());
	REQUIRE(!exp.isHeadSymbol());
}

TEST_CASE("Test symbol expression", "[expression]") {
	Expression exp(Atom("asymbol"));

	REQUIRE(!exp.isHeadNumber());
	REQUIRE(!exp.isHeadComplex());
	REQUIRE(exp.isHeadSymbol());
}
