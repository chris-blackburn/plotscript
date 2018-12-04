#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

#include "threaded_interpreter.hpp"

// ***************** begin interrupt handling *******************
#include <unistd.h>
volatile std::atomic<bool> interrupt_flag;
void interrupt_handler(int signal_num) {
	if(signal_num == SIGINT) {
		if (interrupt_flag) {
			exit(EXIT_FAILURE);
		}

		interrupt_flag = true;
	}
}

inline void install_handler() {
	struct sigaction sigIntHandler;

	sigIntHandler.sa_handler = interrupt_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;

	sigaction(SIGINT, &sigIntHandler, NULL);
}

// ***************** end interrupt handling *******************

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

int eval_from_stream(std::istream& stream) {

	// evaluate the stream directly
	OutputQueue oq;
	ThreadedInterpreter interp(&oq, stream);

	// wait for the output
	OutputMessage msg;
	oq.wait_pop(msg);

	if (msg.type == ErrorType) {
		error(msg.err);
		return EXIT_FAILURE;
	} else if (msg.type == ExpressionType) {
		std::cout << msg.exp << std::endl;
		return EXIT_SUCCESS;
	}

	return EXIT_FAILURE;
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

// A REPL is a repeated read-eval-print loop
void repl() {
	InputQueue iq;
	OutputQueue oq;
	ThreadedInterpreter interp(&iq, &oq);

	wait_for_startup(interp, oq);
	while (!std::cin.eof()) {
		interrupt_flag = false;

		prompt();
		std::string line = readline();

		if (line.empty()) {
			continue;
		}

		// Handle thread control
		if (line == "%start") {
			interp.start();
			continue;
		} else if (line == "%stop") {
			interp.stop();
			continue;
		} else if (line == "%reset") {
			interp.reset();
			continue;
		} else if (line == "%exit") {
			exit(EXIT_SUCCESS);
		}

		// if the interpreter thread is not active, output an error
		if (interp.isActive()) {

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
		} else {
			error("interpreter kernel not running");
		}
	}
}

int main(int argc, char* argv[]) {
	install_handler();

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
