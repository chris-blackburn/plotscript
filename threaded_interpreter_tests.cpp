#include "catch.hpp"

#include "threaded_interpreter.hpp"
#include <thread>

Expression run_threaded(const std::string& program, bool runSemantic = false) {
	InputQueue iq;
	OutputQueue oq;
	ThreadedInterpreter interp(&iq, &oq);

	// run the program by queueing it
	iq.push(program);

	// wait for an output
	OutputMessage msg;
	oq.wait_pop(msg);

	// Once the output has been recieved, check if it was an error or expression
	if (msg.type == ExpressionType) {
		return msg.exp;
	} else if (msg.type == ErrorType) {

		// If an error was thrown, then run semantic should be true (if intended to error)
		REQUIRE(runSemantic);
	}

	// just here for the sake of returning something
	return Expression();
}

TEST_CASE("Constructs and stops on destruction", "[ThreadedInterpreter]") {
	InputQueue iq;
	OutputQueue oq;

	ThreadedInterpreter interp(&iq, &oq);
}

TEST_CASE("Test simple expressions", "[ThreadedInterpreter]") {

	{
		std::string program = "(/ 1 2)";
		INFO(program);
		Expression result = run_threaded(program);
		REQUIRE(result == Expression(0.5));
	}
}

TEST_CASE("Test error catching", "[ThreadedInterpreter]") {

	{
		std::string program = "(/ 1 +)";
		INFO(program);
		Expression result = run_threaded(program, true);
	}

	{
		std::string program = ")";
		INFO(program);
		Expression result = run_threaded(program, true);
	}
}

TEST_CASE("Test reset capability", "[ThreadedInterpreter]") {

	{
		// when a reset happens, all environment data should be reset
		InputQueue iq;
		OutputQueue oq;
		ThreadedInterpreter interp(&iq, &oq);

		// define some variable, and make sure it's retrievable
		iq.push("(define a 22)");
		iq.push("(a)");

		// Grab the second output
		OutputMessage msg;
		oq.wait_pop(msg);
		oq.wait_pop(msg);
		REQUIRE(msg.exp == Expression(22));

		// reset the kernel
		interp.reset();

		// check to make sure the environment was reset
		iq.push("(a)");
		oq.wait_pop(msg);
		REQUIRE(msg.type == ErrorType);
	}
}
