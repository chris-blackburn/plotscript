#include "catch.hpp"

#include "threaded_interpreter.hpp"
#include <thread>

void waitForStartup(ThreadedInterpreter& interp, OutputQueue& oq) {
	while (!interp.isStartupLoaded()) {}
	OutputMessage startupMsg;
	oq.try_pop(startupMsg);
}

Expression run_threaded(const std::string& program, bool runSemantic = false) {
	InputQueue iq;
	OutputQueue oq;
	ThreadedInterpreter interp(&iq, &oq);
	waitForStartup(interp, oq);

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

TEST_CASE("Direct stream evaluation", "[ThreadedInterpreter]") {
	std::string program = "(+ 4 3)";
	std::istringstream stream(program);

	// evaluate the stream directly
	OutputQueue oq;
	ThreadedInterpreter interp(&oq, stream);

	// wait for the output
	OutputMessage msg;
	oq.wait_pop(msg);

	REQUIRE(msg.exp == Expression(7));
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
		waitForStartup(interp, oq);

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

TEST_CASE("Make sure the startup file loads", "[ThreadedInterpreter]") {

	{ // points
		InputQueue iq;
		OutputQueue oq;

		ThreadedInterpreter interp(&iq, &oq);
		waitForStartup(interp, oq);

		iq.push("(define p (make-point 17 5))");
		iq.push("(get-property \"object-name\" p)");
		iq.push("(get-property \"size\" p)");

		OutputMessage msg;
		oq.wait_pop(msg);
		REQUIRE(msg.exp == Expression(std::vector<Expression>{Expression(17), Expression(5)}));
		oq.wait_pop(msg);
		REQUIRE(msg.exp == Expression(Atom("\"point\"")));
		oq.wait_pop(msg);
		REQUIRE(msg.exp == Expression(0));
	}

	{ // lines
		InputQueue iq;
		OutputQueue oq;

		ThreadedInterpreter interp(&iq, &oq);
		waitForStartup(interp, oq);

		iq.push("(define l (make-line (list 17 5) (list 22 22)))");
		iq.push("(get-property \"object-name\" l)");
		iq.push("(get-property \"thickness\" l)");

		OutputMessage msg;
		oq.wait_pop(msg);
		oq.wait_pop(msg);
		REQUIRE(msg.exp == Expression(Atom("\"line\"")));
		oq.wait_pop(msg);
		REQUIRE(msg.exp == Expression(1));
	}

	{ // text
		InputQueue iq;
		OutputQueue oq;

		ThreadedInterpreter interp(&iq, &oq);
		waitForStartup(interp, oq);

		iq.push("(define t (make-text \"Hi\"))");
		iq.push("(get-property \"object-name\" t)");
		iq.push("(get-property \"position\" t)");
		iq.push("(get-property \"text-scale\" t)");
		iq.push("(get-property \"text-rotation\" t)");

		OutputMessage msg;
		oq.wait_pop(msg);
		REQUIRE(msg.exp == Expression(Atom("\"Hi\"")));
		oq.wait_pop(msg);
		REQUIRE(msg.exp == Expression(Atom("\"text\"")));
		oq.wait_pop(msg);
		REQUIRE(msg.exp == Expression(std::vector<Expression>{Expression(0), Expression(0)}));
		oq.wait_pop(msg);
		REQUIRE(msg.exp == Expression(1));
		oq.wait_pop(msg);
		REQUIRE(msg.exp == Expression(0));
	}
}
