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

	return Expression();
}

TEST_CASE("Constructs and stops on destruction", "[ThreadedInterpreter]") {
	InputQueue iq;
	OutputQueue oq;

	ThreadedInterpreter interp(&iq, &oq);
}

TEST_CASE("Test valid expressions", "[ThreadedInterpreter]") {

	{
		std::string program = "(/ 1 2)";
		INFO(program);
		Expression result = run_threaded(program);
		REQUIRE(result == Expression(0.5));
	}
}
