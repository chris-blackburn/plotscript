#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

#include "threaded_interpreter.hpp"

void prompt() {
	std::cout << "\nplotscript> ";
}

std::string readline() {
	std::string line;
	std::getline(std::cin, line);

	return line;
}

void error(const std::string& err_str) {
	std::cerr << "Error: " << err_str << std::endl;
}

void info(const std::string& err_str) {
	std::cout << "Info: " << err_str << std::endl;
}

void load_startup_file(Interpreter& interp) {
	std::ifstream ifs(STARTUP_FILE);

	if (!ifs) {
		error("Could not open startup file for reading.");
		return;
	}

	if (!interp.parseStream(ifs)) {
		error("Invalid Program in startup file. Could not parse.");
	} else {
		try {
			interp.evaluate();
		} catch (const SemanticError& ex) {
			std::cerr << ex.what() << " [startup]" << std::endl;
		}
	}
}

int eval_from_stream(std::istream& stream) {
	Interpreter interp;

	load_startup_file(interp);
	if (!interp.parseStream(stream)) {
		error("Invalid Program. Could not parse.");
		return EXIT_FAILURE;
	} else {
		try {
			Expression exp = interp.evaluate();
			std::cout << exp << std::endl;
		} catch (const SemanticError& ex) {
			std::cerr << ex.what() << std::endl;
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

int eval_from_file(std::string filename) {
	std::ifstream ifs(filename);

	if (!ifs) {
		error("Could not open file for reading.");
		return EXIT_FAILURE;
	}

	return eval_from_stream(ifs);
}

int eval_from_command(std::string argexp) {
	std::istringstream expression(argexp);

	return eval_from_stream(expression);
}

void wait_for_startup(ThreadedInterpreter& interp, OutputQueue& oq) {

	// Wait until the startup file gets loaded. If an error occured, there will be an error message
	// in the output queue
	while (!interp.isStartupLoaded()) {}
	OutputMessage startupMsg;
	oq.try_pop(startupMsg);

	if (startupMsg.type == ErrorType) {
		error(startupMsg.err);
	}
}

// A REPL is a repeated read-eval-print loop
void repl() {
	InputQueue iq;
	OutputQueue oq;
	ThreadedInterpreter interp(&iq, &oq);

	wait_for_startup(interp, oq);
	while (!std::cin.eof()) {

		prompt();
		std::string line = readline();

		if (line.empty()) {
			continue;
		}

		// TODO: Test if the input was %start, %stop, or %reset and handle
		// TODO: handle if interp is not running

		// queue the user's expression
		iq.push(line);

		// wait for an output
		OutputMessage(msg);
		oq.wait_pop(msg);

		// Output the message
		if (msg.type == ErrorType) {
			error(msg.err);
		} else if (msg.type == ExpressionType) {
			std::cout << msg.exp << std::endl;
		}
	}
}

int main(int argc, char* argv[]) {
	if (argc == 2) {
		return eval_from_file(argv[1]);
	} else if (argc == 3) {
		if (std::string(argv[1]) == "-e") {
			return eval_from_command(argv[2]);
		} else {
			error("Incorrect number of command line arguments.");
		}
	} else {
		repl();
	}

	return EXIT_SUCCESS;
}
