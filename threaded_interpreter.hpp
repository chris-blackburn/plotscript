#ifndef THREADED_INTERPRETER_HPP
#define THREADED_INTERPRETER_HPP

#include "interpreter.hpp"
#include "message_queue.hpp"

// The output could be an expression or error, so we need to handle both cases
enum _OutputMessageType {Expression, Error};
typedef struct _OutputMessage {
	_OutputMessageType type;
	Expression exp;
	SemanticError err;

	// constructors for use in container emplace
	_OutputMessage() {};
	_OutputMessage(_OutputMessageType t, Expression e): type(t), exp(e) {};
	_OutputMessage(_OutputMessageType t, SemanticError e): type(t), err(e) {};
} OutputMessage;

// Create message types for the input and output queues
typedef MessageQueue<std::istream&> InputQueue;
typedef MessageQueue<OutputMessage> OutputQueue;

// This is a wrapper class for the interpreter. It is meant to be run in a separate thread and
// contains its own event loop (via the overloaded () operator)
class ThreadedInterpreter {
public:
	ThreadedInterpreter(InputQueue* im, OutputQueue* om);

	// The main event loop will be called by the parent thread
	void operator()();
private:
	Interpreter m_interp;

	InputQueue* m_iq;
	OutputQueue* m_oq;
};

#endif
