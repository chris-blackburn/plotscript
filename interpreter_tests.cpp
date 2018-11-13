#include "catch.hpp"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

#include "semantic_error.hpp"
#include "interpreter.hpp"
#include "expression.hpp"

Expression run(const std::string& program, bool runSemantic = false) {
	std::istringstream iss(program);

	Interpreter interp;

	bool ok = interp.parseStream(iss);
	if (!ok) {
		std::cerr << "Failed to parse: " << program << std::endl;
	}

	REQUIRE(ok == true);

	Expression result;
	if (runSemantic) {
		REQUIRE_THROWS_AS(result = interp.evaluate(), SemanticError);
	} else {
		REQUIRE_NOTHROW(result = interp.evaluate());
	}

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

TEST_CASE("Test Interpreter result with simple procedures (div)", "[interpreter]") {

	{ // binary case
		std::string program = "(/ 1 2)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(0.5));
	}

	{ // complex numbers, 2-ary, one complex number
		std::string program = "(/ I 2)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(complex(0, 0.5)));
	}

	{ // complex numbers, 4-ary, 3 complex number
		std::string program = "(/ I I)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(complex(1, 0)));
	}

	{ // Inverse
		std::string program = "(/ 2)";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(0.5));
	}

	{ // Inverse of complex
		std::string program = "(/ (* I 1))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(complex(0, -1)));
	}

	{
		INFO("Should throw semantic error for:");
		std::vector<std::string> programs = {
			"(/ 1 2 3)",
			"(/ (list 1))",
			"(/ \"eggs\" 1)",
			"(/ \"eggs\")"
		};

		for (auto s : programs) {
			INFO(s);
			run(s, true);
		}
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

	{
		std::string program = "(begin)";
		INFO(program);
		run(program, true);
	}

	{
		std::string program = "(define a (* 1 I))";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(complex(0, 1)));
	}

	{
		std::string program = "(define a \"eggs\")";
		INFO(program);
		Expression result = run(program);
		REQUIRE(result == Expression(Atom("\"eggs\"")));
	}

	{
		INFO("Should throw semantic error for:");
		std::vector<std::string> programs = {
			"(begin (define a 1) (define a 2))",
		};

		for (auto s : programs) {
			INFO(s);
			run(s, true);
		}
	}
}

TEST_CASE("Test Interpreter special forms: list", "[interpreter]") {

	{
		std::string program = "(list)";
		INFO(program);
		Expression result = run(program);

		std::vector<Expression> expected;
		REQUIRE(result == Expression(expected));
	}

	{
		std::string program = "(begin (define mylist (list 1)) (list mylist))";
		INFO(program);
		Expression result = run(program);

		std::vector<Expression> expected = {Expression(std::vector<Expression>{Expression(1)})};
		REQUIRE(result == Expression(expected));
	}

	{
		std::string program = "(list (* I 1) (sqrt 4) 42 (list) (list 10 (/ 1 2)))";
		INFO(program);
		Expression result = run(program);

		std::vector<Expression> expected = {Expression(complex(0, 1)), Expression(2), Expression(42),
			std::vector<Expression>({}), Expression(std::vector<Expression>{Expression(10),
				Expression(0.5)})};
		REQUIRE(result == Expression(expected));
	}

	{
		INFO("Should throw semantic error for:");
		std::vector<std::string> programs = {
			"(+ (list 1) 1)",
			"(- (list 1) 1)",
			"(- (list 1))",
			"(* (list 1) 1)",
			"(/ (list 1) 1)",
			"(sqrt (list 1))",
			"(sqrt (list (* 1 I)))",
			"(^ (list 1) 2)",
			"(^ (list (* 1 I)))",
			"(ln (list 1))",
			"(cos (list 1))",
			"(sin (list 1))",
			"(tan (list 1))",
			"(real (list (* 1 I)))",
			"(imag (list (* 1 I)))",
			"(mag (list (* 1 I)))",
			"(arg (list (* 1 I)))",
			"(conj (list (* 1 I)))"
		};

		for (auto s : programs) {
			INFO(s);
			run(s, true);
		}
	}
}

TEST_CASE("Test Interpreter special forms: lambda", "[interpreter]") {

	{ // Simple lambda
		std::string program = "(begin (define f (lambda (x) (- x))) (f 7))";
		INFO(program);
		Expression result = run(program);

		REQUIRE(result == Expression(-7));
	}

	{ // Shadowing test
		std::string program = "(begin (define x 3) (define f (lambda (x) (- x))) (f 7))";
		INFO(program);
		Expression result = run(program);

		REQUIRE(result == Expression(-7));
	}

	{ // Capture test
		std::string program = "(begin (define a 3) (define f (lambda (x) (- a x))) (f 1))";
		INFO(program);
		Expression result = run(program);

		REQUIRE(result == Expression(2));
	}

	{ // multiple arguments
		std::string program = "(begin (define f (lambda (x y) (- y x))) (f 1 2))";
		INFO(program);
		Expression result = run(program);

		REQUIRE(result == Expression(1));
	}

	{ // More complicated expression
		std::string program = "(begin (define f (lambda (x y) (+ 1 (- x (* y I))))) (f 1 2))";
		INFO(program);
		Expression result = run(program);

		REQUIRE(result == Expression(complex(2, -2)));
	}

	{ // Setting the property of a lambda function's return value shouldn't modify the lambda itself
		std::string program = "(begin "
		"(define inc (lambda (x) (+ 1 x))) "
		"(set-property \"name\" \"inc\" (inc 1)) "
		"(inc 7)"
		")";
		INFO(program);
		Expression result = run(program);

		REQUIRE(result == Expression(8));
	}

	{ // Set the property of a lambda function, and still use the lambda function
		std::string program = "(begin "
		"(define inc (lambda (x) (+ 1 x))) "
		"(set-property \"name\" \"inc\" (inc)) "
		"(inc 7)"
		")";
		INFO(program);
		Expression result = run(program);

		REQUIRE(result == Expression(8));
	}

	{ // Set the property of a lambda function
		std::string program = "(begin "
		"(define inc (lambda (x) (+ 1 x))) "
		"(set-property \"name\" \"inc\" (inc)) "
		"(inc)"
		")";
		INFO(program);
		Expression result = run(program);
		Expression name = result.getProperty("name");

		REQUIRE(name == Expression(Atom("\"inc\"")));
	}

	{
		INFO("Should throw semantic error for:");
		std::vector<std::string> programs = {
			"(lambda)",
			"(lambda (1))",
			"(lambda (x))",
			"(lambda (x) (1) (1))",
			"(begin (define f (lambda (x) (- x))) (f 7 1))",
			"(lambda (+) (+ + 1))",
			"(lambda (8) (+ 8 1))"
		};

		for (auto s : programs) {
			INFO(s);
			run(s, true);
		}
	}
}

TEST_CASE("Test Interpreter special forms: set-property", "[interpreter]") {

	{
		std::string program = "(set-property \"number\" \"one\" (+ 0 1))";
		INFO(program);
		Expression result = run(program);

		REQUIRE(result == Expression(1));
	}

	{
		std::string program = "(set-property \"number\" 1 \"MyNumber\")";
		INFO(program);
		Expression result = run(program);

		REQUIRE(result == Expression(Atom("\"MyNumber\"")));
	}

	{
		std::string program = "(begin (define a \"hi\") (set-property \"number\" 1 \"hi\"))";
		INFO(program);
		Expression result = run(program);

		REQUIRE(result == Expression(Atom("\"hi\"")));
	}

	{ // test the functional set property
		std::string program = "(1)";
		INFO(program);
		Expression result = run(program);

		result.setProperty("number", Expression(Atom("\"one\"")));
		REQUIRE(result.getProperty("number") == Expression(Atom("\"one\"")));
	}

	{
		INFO("Should throw semantic error for:");
		std::vector<std::string> programs = {
			"(define set-property 10)",
			"(set-property)",
			"(set-property \"number\")",
			"(set-property \"number\" \"one\")",
			"(set-property \"number\" \"one\" 1 1)",
			"(set-property 1 \"one\" 1)"
		};

		for (auto s : programs) {
			INFO(s);
			run(s, true);
		}
	}
}

TEST_CASE("Test Interpreter special forms: get-property", "[interpreter]") {

	{ // try to get a property from anonymous expression with no properties
		std::string program = "(get-property \"note\" (+ 1 2))";
		INFO(program);
		Expression result = run(program);

		REQUIRE(result == Expression());
	}

	{ // try to get a property from anonymous expression with properties
		std::string program = "(get-property \"note\" "
			"(set-property \"note\" \"a complex number\" (+ 1 I)))";
		INFO(program);
		Expression result = run(program);

		REQUIRE(result == Expression(Atom("\"a complex number\"")));
	}

	{ // try to get a property from something defined without setting the property
		std::string program = "(begin "
			"(define a (+ 1 I)) "
			"(get-property \"note\" a))";
		INFO(program);
		Expression result = run(program);

		REQUIRE(result == Expression());
	}

	{ // get an existing property from something defined
		std::string program = "(begin "
			"(define a (+ 1 I)) "
			"(define b (set-property \"note\" \"a complex number\" a)) "
			"(get-property \"note\" b))";
		INFO(program);
		Expression result = run(program);

		REQUIRE(result == Expression(Atom("\"a complex number\"")));
	}

	{ // try to get an invalid property from something defined that has properties
		std::string program = "(begin "
			"(define a (+ 1 I)) "
			"(define b (set-property \"note\" \"a complex number\" a)) "
			"(get-property \"foo\" b))";
		INFO(program);
		Expression result = run(program);

		REQUIRE(result == Expression());
	}

	{ // Overwrite old property
		std::string program = "(begin "
			"(define b (set-property \"note\" \"a number\" (+ 1 I))) "
			"(set-property \"note\" \"a *complex number\" b) "
			"(get-property \"note\" b))";
		INFO(program);
		Expression result = run(program);

		REQUIRE(result == Expression(Atom("\"a *complex number\"")));
	}

	{ // Test the get property public function for expressions
		std::string program = "(set-property \"note\" \"a number\" 3)";
		INFO(program);
		Expression result = run(program);

		REQUIRE(result.getProperty("note") == Expression(Atom("\"a number\"")));
	}

	{ // Test the get property public function for expressions, no property
		std::string program = "(set-property \"note\" \"a number\" 3)";
		INFO(program);
		Expression result = run(program);

		REQUIRE(result.getProperty("not") == Expression());
	}

	{ // Properties should not carry over to new expressions (expression copy assignment)
		std::string program = "(begin "
			"(define a (set-property \"note\" "
			"(set-property \"what\" \"some text\" \"a number\") 3))"
			"(set-property \"note\" \"three\" a)"
			"(get-property \"what\" (get-property \"note\" a))"
			")";
		INFO(program);
		Expression result = run(program);

		REQUIRE(result == Expression());
	}

	{
		INFO("Should throw semantic error for:");
		std::vector<std::string> programs = {
			"(define get-property 10)",
			"(get-property)",
			"(get-property \"number\")",
			"(get-property 1 \"one\")"
		};

		for (auto s : programs) {
			INFO(s);
			run(s, true);
		}
	}
}

TEST_CASE("Testing list specific functions (first)", "[Interpreter]") {

	{
		std::string program = "(first (list 1 2 3))";
		INFO(program);
		Expression result = run(program);

		REQUIRE(result == Expression(1));
	}

	{
		std::string program = "(first (list (list 1) 2 3))";
		INFO(program);
		Expression result = run(program);

		std::vector<Expression> expected = {Expression(1)};
		REQUIRE(result == Expression(expected));
	}

	{
		std::string program = "(first (list \"eggs\" 2 3))";
		INFO(program);
		Expression result = run(program);

		REQUIRE(result == Expression(Atom("\"eggs\"")));
	}

	{
		INFO("Should throw semantic error for:");
		std::vector<std::string> programs = {
			"(first (list 1) 1)",
			"(first (list))",
			"(first (1))",
			"(first (+ 17 (* 1 I)))"
		};

		for (auto s : programs) {
			INFO(s);
			run(s, true);
		}
	}
}

TEST_CASE("Testing list specific functions (rest)", "[Interpreter]") {

	{
		std::string program = "(rest (list 1 2 3))";
		INFO(program);
		Expression result = run(program);

		std::vector<Expression> expected = {Expression(2), Expression(3)};
		REQUIRE(result == Expression(expected));
	}

	{
		std::string program = "(rest (list 1 (list 2)))";
		INFO(program);
		Expression result = run(program);

		std::vector<Expression> expected = {Expression(std::vector<Expression>{Expression(2)})};
		REQUIRE(result == Expression(expected));
	}

	{
		std::string program = "(rest (list 1))";
		INFO(program);
		Expression result = run(program);

		std::vector<Expression> expected;
		REQUIRE(result == Expression(expected));
	}

	{
		std::string program = "(rest (list 1 \"eggs\" \"milk\"))";
		INFO(program);
		Expression result = run(program);

		std::vector<Expression> expected = {Expression(Atom("\"eggs\"")),
			Expression(Atom("\"milk\""))};
		REQUIRE(result == Expression(expected));
	}

	{
		INFO("Should throw semantic error for:");
		std::vector<std::string> programs = {
			"(rest (list 1) 1)",
			"(rest (list))",
			"(rest (1))",
			"(rest (+ 17 (* 1 I)))"
		};

		for (auto s : programs) {
			INFO(s);
			run(s, true);
		}
	}
}

TEST_CASE("Testing list specific functions (length)", "[Interpreter]") {

	{
		std::string program = "(length (list 1 2 3))";
		INFO(program);
		Expression result = run(program);

		REQUIRE(result == Expression(3));
	}

	{
		std::string program = "(length (list 1 (list 2)))";
		INFO(program);
		Expression result = run(program);

		REQUIRE(result == Expression(2));
	}

	{
		std::string program = "(length (list))";
		INFO(program);
		Expression result = run(program);

		std::vector<Expression> expected;
		REQUIRE(result == Expression(0));
	}

	{
		std::string program = "(length (list \"sausage\" \"eggs\" \"milk\"))";
		INFO(program);
		Expression result = run(program);

		std::vector<Expression> expected;
		REQUIRE(result == Expression(3));
	}

	{
		INFO("Should throw semantic error for:");
		std::vector<std::string> programs = {
			"(length (list 1) 1)",
			"(length (1))",
			"(length (+ 17 (* 1 I)))"
		};

		for (auto s : programs) {
			INFO(s);
			run(s, true);
		}
	}
}

TEST_CASE("Testing list specific functions (append)", "[Interpreter]") {
	{
		std::string program = "(append (list) (1))";
		INFO(program);
		Expression result = run(program);

		std::vector<Expression> expected = {Expression(1)};
		REQUIRE(result == Expression(expected));
	}

	{
		std::string program = "(append (list) (list 1))";
		INFO(program);
		Expression result = run(program);

		std::vector<Expression> expected = {Expression(std::vector<Expression>{Expression(1)})};
		REQUIRE(result == Expression(expected));
	}

	{
		std::string program = "(append (list \"eggs\") \"milk\")";
		INFO(program);
		Expression result = run(program);

		std::vector<Expression> expected = {Expression(Atom("\"eggs\"")),
			Expression(Atom("\"milk\""))};
		REQUIRE(result == Expression(expected));
	}

	{
		INFO("Should throw semantic error for:");
		std::vector<std::string> programs = {
			"(append (2) (list))",
			"(append (list) (2) (3))",
			"(append (+ 17 (* 1 I)))"
		};

		for (auto s : programs) {
			INFO(s);
			run(s, true);
		}
	}
}

TEST_CASE("Testing list specific functions (join)", "[Interpreter]") {
	{
		std::string program = "(join (list) (list 1))";
		INFO(program);
		Expression result = run(program);

		std::vector<Expression> expected = {Expression(1)};
		REQUIRE(result == Expression(expected));
	}

	{
		std::string program = "(join (list (* I 2)) (list 1))";
		INFO(program);
		Expression result = run(program);

		std::vector<Expression> expected = {Expression(complex(0, 2)), Expression(1)};
		REQUIRE(result == Expression(expected));
	}

	{
		std::string program = "(join (list \"eggs\") (list \"milk\"))";
		INFO(program);
		Expression result = run(program);

		std::vector<Expression> expected = {Expression(Atom("\"eggs\"")),
			Expression(Atom("\"milk\""))};
		REQUIRE(result == Expression(expected));
	}

	{
		INFO("Should throw semantic error for:");
		std::vector<std::string> programs = {
			"(join (list) (2) (3))",
			"(join (list) (2))",
			"(join (2) (list))",
			"(join (+ 17 (* 1 I)))"
		};

		for (auto s : programs) {
			INFO(s);
			run(s, true);
		}
	}
}

TEST_CASE("Testing list specific functions (range)", "[Interpreter]") {
	{
		std::string program = "(range 0 2 1)";
		INFO(program);
		Expression result = run(program);

		std::vector<Expression> expected = {Expression(0), Expression(1), Expression(2)};
		REQUIRE(result == Expression(expected));
	}

	{
		std::string program = "(range 0 .22 .11)";
		INFO(program);
		Expression result = run(program);

		std::vector<Expression> expected = {Expression(0), Expression(.11), Expression(.22)};
		REQUIRE(result == Expression(expected));
	}

	{
		std::string program = "(range -1 1 1)";
		INFO(program);
		Expression result = run(program);

		std::vector<Expression> expected = {Expression(-1), Expression(0), Expression(1)};
		REQUIRE(result == Expression(expected));
	}

	{
		std::string program = "(range -4 -2 1)";
		INFO(program);
		Expression result = run(program);

		std::vector<Expression> expected = {Expression(-4), Expression(-3), Expression(-2)};
		REQUIRE(result == Expression(expected));
	}

	{
		std::string program = "(range 2 5 4)";
		INFO(program);
		Expression result = run(program);

		std::vector<Expression> expected = {Expression(2)};
		REQUIRE(result == Expression(expected));
	}

	{
		INFO("Should throw semantic error for:");
		std::vector<std::string> programs = {
			"(range (list) (2) (2))",
			"(range (2) (list) (2))",
			"(range (2) (2) (list))",
			"(range (1))",
			"(range (1) (3))",
			"(range (2) (1) (1))",
			"(range (2) (3) (-1))",
			"(range (list \"eggs\"))"
		};

		for (auto s : programs) {
			INFO(s);
			run(s, true);
		}
	}
}

TEST_CASE("Testing functional procedure (apply)", "[interpreter]") {

	{ // standard procedure
		std::string program = "(apply + (list 1 2 3 4))";
		INFO(program);
		Expression result = run(program);

		REQUIRE(result == Expression(10));
	}

	{ // defined lambda function
		std::string program = "(begin (define f (lambda (x) (list x (+ x 1)))) (apply f (list 7)))";
		INFO(program);
		Expression result = run(program);

		std::vector<Expression> expected = {Expression(7), Expression(8)};
		REQUIRE(result == Expression(expected));
	}

	{ // anonymous lambda function
		std::string program = "(apply (lambda (x) (list x (+ x 1))) (list 7))";
		INFO(program);
		Expression result = run(program);

		std::vector<Expression> expected = {Expression(7), Expression(8)};
		REQUIRE(result == Expression(expected));
	}

	{
		INFO("Should throw semantic error for:");
		std::vector<std::string> programs = {
			"(apply)",
			"(apply +)",
			"(apply + (list 1) (1))",
			"(apply + (1))",
			"(apply 1 (list 1))",
			"(apply e (list 1))",
			"(apply (+ z I) (list 0))"
		};

		for (auto s : programs) {
			INFO(s);
			run(s, true);
		}
	}
}

TEST_CASE("Testing functional procedure (map)", "[interpreter]") {

	{ // standard procedure
		std::string program = "(map - (list 1 2))";
		INFO(program);
		Expression result = run(program);

		std::vector<Expression> expected = {Expression(-1), Expression(-2)};
		REQUIRE(result == Expression(expected));
	}

	{ // pre-defined lambda
		std::string program = "(begin (define f (lambda (x) (+ 1 x))) (map f (list 1 2)))";
		INFO(program);
		Expression result = run(program);

		std::vector<Expression> expected = {Expression(2), Expression(3)};
		REQUIRE(result == Expression(expected));
	}

	{ // anonymous lambda
		std::string program = "(map (lambda (x) (+ 1 x)) (list 1 2))";
		INFO(program);
		Expression result = run(program);

		std::vector<Expression> expected = {Expression(2), Expression(3)};
		REQUIRE(result == Expression(expected));
	}

	{
		INFO("Should throw semantic error for:");
		std::vector<std::string> programs = {
			"(map)",
			"(map +)",
			"(map + (list 1) (1))",
			"(map + (1))",
			"(map 1 (list 1))",
			"(map e (list 1))",
			"(map (+ z I) (list 0))"
		};

		for (auto s : programs) {
			INFO(s);
			run(s, true);
		}
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

	{
		INFO("Should throw semantic error for:");
		std::vector<std::string> programs = {
			"(ab 1 2)",
			"(+ \"eggs\" 1)",
			"(- \"eggs\" 1)",
			"(- \"eggs\")",
			"(* \"eggs\" 1)"
		};

		for (auto s : programs) {
			INFO(s);
			run(s, true);
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
			"(sqrt \"eggs\")"
		};

		for (auto s : programs) {
			INFO(s);
			run(s, true);
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
			"(^ 1 2 3)",
			"(^ \"eggs\" 2)"
		};

		for (auto s : programs) {
			INFO(s);
			run(s, true);
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
			"(ln -1)",
			"(ln \"eggs\")"
		};

		for (auto s : programs) {
			INFO(s);
			run(s, true);
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
			"(sin 1 2)",
			"(sin \"eggs\")"
		};

		for (auto s : programs) {
			INFO(s);
			run(s, true);
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
			run(s, true);
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
			"(tan 1 2)",
			"(tan \"eggs\")"
		};

		for (auto s : programs) {
			INFO(s);
			run(s, true);
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
			"(real \"eggs\")",
			"(imag \"eggs\")",
			"(mag \"eggs\")",
			"(arg \"eggs\")",
			"(conj \"eggs\")"
		};

		for (auto s : programs) {
			INFO(s);
			run(s, true);
		}
	}
}

TEST_CASE("Simple discrete plot tests", "[interpreter]") {

	// Test redefinition, wrong number of arguments
	// Each should throw sematic error
	{
		INFO("Should throw semantic error for:");
		std::vector<std::string> programs = {
			"(define discrete-plot 1)",
			"(discrete-plot 1)",
			"(discrete-plot (list))",
			"(discrete-plot (list (List 1 2)))",
			"(discrete-plot (list (list 1 I) (list 2 3)))",
			"(discrete-plot (list (list 1 2) (list 2 3)) 1)",
			"(discrete-plot (list (list 1 2 3) (list 2 3)))",
			"(discrete-plot (list (list 1) (list 2 3)))",
			"(discrete-plot (list (list 1 2) (list 2 3)) (list) (list))"
		};

		for (auto s : programs) {
			INFO(s);
			run(s, true);
		}
	}
}

TEST_CASE("Test some semantically invalid expresions", "[interpreter]") {
	std::vector<std::string> programs = {
		"(@ none)", // so such procedure
		"(- 1 1 2)", // too many arguments
		"(define begin 1)", // redefine special form
		"(define define 1)",
		"(define list 1)",
		"(define lambda 1)",
		"(define apply 1)",
		"(define map 1)",
		"(define 1 1)",
		"(define pi 3.14)" // redefine builtin symbol
	};

		for (auto s : programs) {
			INFO(s);
			run(s, true);
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
