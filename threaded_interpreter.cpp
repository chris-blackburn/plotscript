#include "threaded_interpreter.hpp"

ThreadedInterpreter::ThreadedInterpreter(InputQueue* iq, OutputQueue* oq): m_iq(iq), m_oq(oq) {
	start();
}

// evaluate a stream directly
ThreadedInterpreter::ThreadedInterpreter(OutputQueue* oq, std::istream& stream): m_oq(oq) {
	if (!m_thread.joinable()) {
		m_thread = std::thread([this, &stream](){
			Interpreter interp;
			loadStartupFile(interp);

			// pop in case there was an error with the startup file
			OutputMessage msg;
			m_oq->try_pop(msg);
			evalStream(interp, stream);
		});
	}
}

// Be sure to stop and join the thread when falling out of scope
ThreadedInterpreter::~ThreadedInterpreter() {
	stop();
}

void ThreadedInterpreter::start() {
	active = true;
	if (!m_thread.joinable()) {

		// Move a new thread into the class' thread
		m_thread = std::thread(&ThreadedInterpreter::run, this);
	}
}

void ThreadedInterpreter::stop() {
	active = false;
	if (m_thread.joinable()) {
		m_thread.join();
	}
}

void ThreadedInterpreter::reset() {
	stop();
	start();
}

void ThreadedInterpreter::error(const std::string& e) {
	m_oq->push(OutputMessage(ErrorType, e));
}

void ThreadedInterpreter::evalStream(Interpreter& interp, std::istream& stream) {
	if (!interp.parseStream(stream)) {
		error("Invalid Expression. Could not parse.");
	} else {

		// try to evaluate the expression, push the result to the output queue
		try {
			Expression exp = interp.evaluate();
			m_oq->push(OutputMessage(ExpressionType, exp));
		} catch(const SemanticError& ex) {
			error(std::string(ex.what()));
		}
	}
}

void ThreadedInterpreter::run() {
	Interpreter interp;
	loadStartupFile(interp);
	while (active) {

		// Grab input messages as they populate the queue. If not message is returned, then continue
		// iterating
		InputMessage msg;
		if (m_iq->try_pop(msg)) {

			// Attempt to parse the input stream
			std::istringstream stream(msg);
			evalStream(interp, stream);
		}
	}
}

void ThreadedInterpreter::loadStartupFile(Interpreter& interp) {
	std::ifstream ifs(STARTUP_FILE);

	if (!ifs) {
		error("Could not open startup file for reading.");
	} else {

		// Just load the expressions from the startup file into the interpreter. We don't need to do
		// anything with them
		if (!interp.parseStream(ifs)) {
			error("Invalid Program in startup file. Could not parse.");
		} else {
			try {
				interp.evaluate();
			} catch (const SemanticError& ex) {
				error(std::string(ex.what()) + " [startup]");
			}
		}
	}

	startupLoaded = true;
}

bool ThreadedInterpreter::isStartupLoaded() const {
	return startupLoaded;
}
