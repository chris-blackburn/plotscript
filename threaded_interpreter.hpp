#ifndef THREADED_INTERPRETER_HPP
#define THREADED_INTERPRETER_HPP

#include <thread>
#include <fstream>

#include "interpreter.hpp"
#include "semantic_error.hpp"
#include "message_queue.hpp"

#include "startup_config.hpp"

typedef std::string InputMessage;

// The output could be an expression or error, so we need to handle both cases
typedef enum _OutputMessageType {ExpressionType, ErrorType} OutputMessageType;
typedef struct _OutputMessage {
	OutputMessageType type;
	Expression exp;
	std::string err;

	// constructors for use in container emplace
	_OutputMessage() {};
	_OutputMessage(OutputMessageType t, Expression e): type(t), exp(e) {};
	_OutputMessage(OutputMessageType t, std::string e): type(t), err(e) {};
} OutputMessage;

// Create message queue types for the input and output queues
typedef MessageQueue<InputMessage> InputQueue;
typedef MessageQueue<OutputMessage> OutputQueue;

// This is a wrapper class for the interpreter. It controls the interpreter inside of a separate
// thread and provides an api to control the state of that thread
class ThreadedInterpreter {
public:
	ThreadedInterpreter(InputQueue* iq, OutputQueue* oq);

	~ThreadedInterpreter();

	// Not copyable
	ThreadedInterpreter(const ThreadedInterpreter&) = delete;
	ThreadedInterpreter& operator=(const ThreadedInterpreter&) = delete;

	// control the state of the thread
	void start();
	void stop();
	void reset();
private:
	std::thread m_thread;

	InputQueue* m_iq;
	OutputQueue* m_oq;

	// The interpreter thread's main event loop
	bool active = true;
	void run();

	// Load the startup plotscript file. Errors are sent to the output queue
	void loadStartupFile(Interpreter& interp);

	void error(const std::string& e);
};

#endif
