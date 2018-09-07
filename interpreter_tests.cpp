#include "catch.hpp"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

#include "semantic_error.hpp"
#include "interpreter.hpp"
#include "expression.hpp"

Expression run(const std::string& program) {
	std::istringstream iss(program);

	Interpreter interp;

	bool ok = interp.parseStream(iss);
	if (!ok) {
		std::cerr << "Failed to parse: " << program << std::endl;
	}

	REQUIRE(ok == true);

	Expression result;
	REQUIRE_NOTHROW(result = interp.evaluate());

	return result;
}

TEST_CASE("Test Interpreter parser with expected input", "[interpreter]") {
	std::string program = "(begin (define r 10) (* pi (* r r)))";

	std::istringstream iss(program);

	Interpreter interp;

	bool ok = interp.parseStream(iss);

	REQUIRE(ok == true);
}

TEST_CASE("Test Interpreter parser with numerical literals", "[interpreter]") {
	std::vector<std::string> programs = {"(1)", "(+1)", "(+1e+0)", "(1e-0)"};

	for (auto program : programs) {
		std::istringstream iss(program);

		Interpreter interp;

		bool ok = interp.parseStream(iss);

		REQUIRE(ok == true);
	}

	{
		std::istringstream iss("(define x 1abc)");

		Interpreter interp;

		bool ok = interp.parseStream(iss);

		REQUIRE(ok == false);
	}
}

TEST_CASE("Test Interpreter parser with truncated input", "[interpreter]") {

	{
		std::string program = "(f";
		std::istringstream iss(program);

		Interpreter interp;
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == false);
	}

	{
		std::string program = "(begin (define r 10) (* pi (* r r";
		std::istringstream iss(program);

		Interpreter interp;
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == false);
	}
}

TEST_CASE("Test Interpreter parser with extra input", "[interpreter]") {
	std::string program = "(begin (define r 10) (* pi (* r r))) )";
	std::istringstream iss(program);

	Interpreter interp;

	bool ok = interp.parseStream(iss);

	REQUIRE(ok == false);
}

TEST_CASE("Test Interpreter parser with single non-keyword", "[interpreter]") {
	std::string program = "hello";
	std::istringstream iss(program);

	Interpreter interp;

	bool ok = interp.parseStream(iss);

	REQUIRE(ok == false);
}

TEST_CASE("Test Interpreter parser with empty input", "[interpreter]") {
	std::string program;
	std::istringstream iss(program);

	Interpreter interp;

	bool ok = interp.parseStream(iss);

	REQUIRE(ok == false);
}

TEST_CASE("Test Interpreter parser with empty expression", "[interpreter]") {
	std::string program = "( )";
	std::istringstream iss(program);

	Interpreter interp;

	bool ok = interp.parseStream(iss);

	REQUIRE(ok == false);
}

TEST_CASE("Test Interpreter parser with bad number string", "[interpreter]") {
	std::string program = "(1abc)";
	std::istringstream iss(program);

	Interpreter interp;

	bool ok = interp.parseStream(iss);

	REQUIRE(ok == false);
}

TEST_CASE("Test Interpreter parser with incorrect input. Regression Test", "[interpreter]") {
	std::string program = "(+ 1 2) (+ 3 4)";
	std::istringstream iss(program);

	Interpreter interp;

	bool ok = interp.parseStream(iss);

	REQUIRE(ok == false);
}

TEST_CASE("Test Interpreter result with literal expressions", "[interpreter]") {

	{ // Number
		std::string program = "(4)";
		Expression result = run(program);
		REQUIRE(result == Expression(4.));
	}

	{ // Symbol
		std::string program = "(pi)";
		Expression result = run(program);
		REQUIRE(result == Expression(atan2(0, -1)));
	}

	{ // Symbol
		std::string program = "(e)";
		Expression result = run(program);
		REQUIRE(result == Expression(exp(1)));
	}
}

TEST_CASE("Test Interpreter result with simple procedures (add)", "[interpreter]") {

	{ // add, binary case
		std::string program = "(+ 1 2)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(3.));
	}

	{ // add, 3-ary case
		std::string program = "(+ 1 2 3)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(6.));
	}

	{ // add, 6-ary case
		std::string program = "(+ 1 2 3 4 5 6)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(21.));
	}

	{ // complex numbers, 2-ary, one complex number
		std::string program = "(+ I 2)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(complex(2, 1)));
	}

	{ // complex numbers, 4-ary, one complex number
		std::string program = "(+ I 2 3.2 4)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(complex(9.2, 1)));
	}

	{ // complex numbers, 4-ary, 3 complex number
		std::string program = "(+ I I 3 I)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(complex(3, 3)));
	}
}

TEST_CASE("Test Interpreter result with simple procedures (mul)", "[interpreter]") {

	{ // binary case
		std::string program = "(* 1 2)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(2));
	}

	{ // 3-ary case
		std::string program = "(* 1 2 3)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(6));
	}

	{ // 6-ary case
		std::string program = "(* 1 2 3 4 5 6)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(720));
	}

	{ // complex numbers, 2-ary, one complex number
		std::string program = "(* I 2)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(complex(0, 2)));
	}

	{ // complex numbers, 4-ary, one complex number
		std::string program = "(* I 2 3 4)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(complex(0, 24)));
	}

	{ // complex numbers, 4-ary, 3 complex number
		std::string program = "(* I I 3 I)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(complex(0, -3)));
	}
}

TEST_CASE("Test Interpreter special forms: begin and define", "[interpreter]") {

	{
		std::string program = "(define answer 42)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(42.));
	}

	{
		std::string program = "(begin (define answer 42)\n(answer))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(42.));
	}

	{
		std::string program = "(begin (define answer (+ 9 11)) (answer))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(20.));
	}

	{
		std::string program = "(begin (define a 1) (define b 1) (+ a b))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(2.));
	}
}

TEST_CASE("Test a medium-sized expression", "[interpreter]") {

	{
		std::string program = "(+ (+ 10 1) (+ 30 (+ 1 1)))";
		Expression result = run(program);
		REQUIRE(result == Expression(43.));
	}
}

TEST_CASE("Test arithmetic procedures", "[interpreter]") {

	{
		std::vector<std::string> programs = {"(+ 1 -2)",
					 "(+ -3 1 1)",
					 "(- 1)",
					 "(- 1 2)",
					 "(* 1 -1)",
					 "(* 1 1 -1)",
					 "(/ -1 1)",
					 "(/ 1 -1)"};

		for (auto s : programs) {
			Expression result = run(s);
			REQUIRE(result == Expression(-1.));
		}
	}
}

TEST_CASE("Test non-trivial arithmetic procedures with complex numbers", "[interpreter]") {

	{ // Combination of addition and multiplication with complex numbers
		std::string program = "(+ 3 (* 5 I) 7 I)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(complex(10, 6)));
	}

	{ // Combination of addition and multiplication with complex numbers
		std::string program = "(+ (* 3 I) (* 7 I) 1)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(complex(1, 10)));
	}

	{ // negative I
		std::string program = "(- I)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(complex(0, -1)));
	}

	{ // negate addition with I
		std::string program = "(- (+ 7 I))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(complex(-7, -1)));
	}

	{ // subtratcion with multiples of I and real numbers
		std::string program = "(- (- (* 3 I) (* 7 I)) (- 1 (- 17)))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(complex(-18, -4)));
	}

	{ // negation of I * I
		std::string program = "(- (* I I))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(complex(1, 0)));
	}

	{ // Divide I by itself
		std::string program = "(/ I I)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(complex(1, 0)));
	}

	{ // Combination of addition, negation, division and multiplication
		std::string program = "(+ (/ I 20) (* I 3) (- 7) (- I))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(complex(-7, 2.05)));
	}
}

TEST_CASE("Test procedures (square root)", "[interpreter]") {

	{
		std::string program = "(sqrt 0)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(0));
	}

	{
		std::string program = "(sqrt 1)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(1));
	}

	{
		std::string program = "(sqrt 64)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(8));
	}

	{
		std::string program = "(sqrt 2)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(std::sqrt(2)));
	}

	{
		std::string program = "(sqrt -1)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(complex(0, 1)));
	}

	{
		std::string program = "(sqrt -64)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(complex(0, 8)));
	}

	{
		std::string program = "(sqrt -2)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(std::sqrt(complex(-2, 0))));
	}

	{
		std::string program = "(sqrt (+ 9 (* 144 I)))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(std::sqrt(complex(9, 144))));
	}

	// Test redefinition, negative roots, wrong number of arguments
	// Each should throw sematic error
	{
		INFO("Should throw semantic error for:");
		std::vector<std::string> programs = {
			"(define sqrt 1)",
			"(sqrt 1 2)",
			"(sqrt 1 I)",
		};

		for (auto s : programs) {
			INFO(s);
			Interpreter interp;

			std::istringstream iss(s);

			bool ok = interp.parseStream(iss);
			REQUIRE(ok == true);

			REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
		}
	}
}

TEST_CASE("Test procedures (pow)", "[interpreter]") {

	{
		std::string program = "(^ 1 2)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(1));
	}

	{
		std::string program = "(^ 2 1)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(2));
	}

	{
		std::string program = "(^ 2 3)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(8));
	}

	{
		std::string program = "(^ 2 -1)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == run("(/ 1 2)"));
	}

	{
		std::string program = "(^ 4 (/ -1 2))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == run("(/ 1 2)"));
	}

	{
		std::string program = "(^ e 0)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(1));
	}

	{
		std::string program = "(^ I 0)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(complex(1, 0)));
	}

	{
		std::string program = "(^ I 2)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(complex(-1, 0)));
	}

	{
		std::string program = "(^ e (- (* pi I)))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(complex(-1, 0)));
	}

	// Test redefinition, wrong number of arguments
	// Each should throw sematic error
	{
		INFO("Should throw semantic error for:");
		std::vector<std::string> programs = {
			"(define ^ 1)",
			"(^ 1)",
			"(^ 1 2 3)"
		};

		for (auto s : programs) {
			INFO(s);
			Interpreter interp;

			std::istringstream iss(s);

			bool ok = interp.parseStream(iss);
			REQUIRE(ok == true);

			REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
		}
	}
}

TEST_CASE("Test procedures (ln)", "[interpreter]") {

	{
		std::string program = "(ln e)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(1));
	}

	{
		std::string program = "(ln 1)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(0));
	}

	{
		std::string program = "(ln 10)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(std::log(10)));
	}

	{
		std::string program = "(ln (^ e 2))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(2));
	}

	// Test redefinition, wrong number of arguments, negatives
	// Each should throw sematic error
	{
		INFO("Should throw semantic error for:");
		std::vector<std::string> programs = {
			"(define ln 1)",
			"(ln 1 2)",
			"(ln -1)"
		};

		for (auto s : programs) {
			INFO(s);
			Interpreter interp;

			std::istringstream iss(s);

			bool ok = interp.parseStream(iss);
			REQUIRE(ok == true);

			REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
		}
	}
}

TEST_CASE("Test procedures (sin)", "[interpreter]") {

	{
		std::string program = "(sin 0)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(0));
	}

	{
		std::string program = "(sin pi)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(0));
	}

	{
		std::string program = "(sin (/ pi 2))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(1));
	}

	{
		std::string program = "(sin (- pi))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(0));
	}

	{
		std::string program = "(sin (/ pi -2))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(-1));
	}

	// Test redefinition, wrong number of arguments
	// Each should throw sematic error
	{
		INFO("Should throw semantic error for:");
		std::vector<std::string> programs = {
			"(define sin 1)",
			"(sin 1 2)"
		};

		for (auto s : programs) {
			INFO(s);
			Interpreter interp;

			std::istringstream iss(s);

			bool ok = interp.parseStream(iss);
			REQUIRE(ok == true);

			REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
		}
	}
}

TEST_CASE("Test procedures (cos)", "[interpreter]") {

	{
		std::string program = "(cos 0)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(1));
	}

	{
		std::string program = "(cos pi)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(-1));
	}

	{
		std::string program = "(cos (/ pi 2))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(0));
	}

	{
		std::string program = "(cos (- pi))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(-1));
	}

	{
		std::string program = "(cos (/ pi -2))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(0));
	}

	// Test redefinition, wrong number of arguments
	// Each should throw sematic error
	{
		INFO("Should throw semantic error for:");
		std::vector<std::string> programs = {
			"(define cos 1)",
			"(cos 1 2)"
		};

		for (auto s : programs) {
			INFO(s);
			Interpreter interp;

			std::istringstream iss(s);

			bool ok = interp.parseStream(iss);
			REQUIRE(ok == true);

			REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
		}
	}
}

TEST_CASE("Test procedures (tan)", "[interpreter]") {

	{
		std::string program = "(tan 0)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(0));
	}

	{
		std::string program = "(tan pi)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(0));
	}

	// Test redefinition, wrong number of arguments
	// Each should throw sematic error
	{
		INFO("Should throw semantic error for:");
		std::vector<std::string> programs = {
			"(define tan 1)",
			"(tan 1 2)"
		};

		for (auto s : programs) {
			INFO(s);
			Interpreter interp;

			std::istringstream iss(s);

			bool ok = interp.parseStream(iss);
			REQUIRE(ok == true);

			REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
		}
	}
}

TEST_CASE("Test complex-unique procedures", "[interpreter]") {

	{
		std::string program = "(real (+ 7 (* 3 I)))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(7));
	}

	{
		std::string program = "(imag (+ 7 (* 3 I)))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(3));
	}

	{
		std::string program = "(mag (+ 3 (* 4 I)))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(5));
	}

	{
		std::string program = "(arg (+ -1 (* 0 I)))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == run("(pi)"));
	}

	{
		std::string program = "(conj (+ 7 (* 3 I)))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(complex(7, -3)));
	}

	// Test redefinition, wrong number of arguments
	// Each should throw sematic error
	{
		INFO("Should throw semantic error for:");
		std::vector<std::string> programs = {
			"(real 1)",
			"(imag 1)",
			"(mag 1)",
			"(arg 1)",
			"(conj 1)",
			"(real 1 2)",
			"(imag 1 2)",
			"(mag 1 2)",
			"(arg 1 2)",
			"(conj 1 2)",
		};

		for (auto s : programs) {
			INFO(s);
			Interpreter interp;

			std::istringstream iss(s);

			bool ok = interp.parseStream(iss);
			REQUIRE(ok == true);

			REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
		}
	}
}

TEST_CASE("Test some semantically invalid expresions", "[interpreter]") {
	std::vector<std::string> programs = {
		"(@ none)", // so such procedure
		"(- 1 1 2)", // too many arguments
		"(define begin 1)", // redefine special form
		"(define pi 3.14)" // redefine builtin symbol
	};

		for (auto s : programs) {
			Interpreter interp;

			std::istringstream iss(s);

			bool ok = interp.parseStream(iss);
			REQUIRE(ok == true);

			REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
		}
}

TEST_CASE("Test for exceptions from semantically incorrect input", "[interpreter]") {
	std::string input = R"(
(+ 1 a)
)";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);

	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE( "Test malformed define", "[interpreter]" ) {
	std::string input = R"(
(define a 1 2)
)";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);

	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test using number as procedure", "[interpreter]") {
	std::string input = R"(
(1 2 3)
)";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);

	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}
