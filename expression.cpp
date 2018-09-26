#include "expression.hpp"

#include <sstream>

#include "environment.hpp"
#include "semantic_error.hpp"

Expression::Expression() {}

Expression::Expression(const std::vector<Expression>& a): m_head(ListRoot), m_tail(a) {}

Expression::Expression(const Atom& a): m_head(a) {}

// recursive copy
Expression::Expression(const Expression& a) {
	m_head = a.m_head;
	for (auto e : a.m_tail) {
		m_tail.push_back(e);
	}
}

Expression& Expression::operator=(const Expression& a) {

	// prevent self-assignment
	if (this != &a) {
		m_head = a.m_head;
		m_tail.clear();

		for (auto e : a.m_tail) {
			m_tail.push_back(e);
		}
	}

	return *this;
}

Atom& Expression::head() {
	return m_head;
}

const Atom& Expression::head() const {
	return m_head;
}

bool Expression::isHeadNumber() const noexcept {
	return m_head.isNumber();
}

bool Expression::isHeadComplex() const noexcept {
	return m_head.isComplex();
}

bool Expression::isHeadSymbol() const noexcept {
	return m_head.isSymbol();
}

bool Expression::isHeadListRoot() const noexcept {
	return m_head == ListRoot;
}

bool Expression::isHeadLambdaRoot() const noexcept {
	return m_head == LambdaRoot;
}

void Expression::append(const Atom& a) {
	m_tail.emplace_back(a);
}

Expression* Expression::tail() {
	Expression* ptr = nullptr;

	if (m_tail.size() > 0) {
		ptr = &m_tail.back();
	}

	return ptr;
}

Expression::ConstIteratorType Expression::tailConstBegin() const noexcept {
	return m_tail.cbegin();
}

Expression::ConstIteratorType Expression::tailConstEnd() const noexcept {
	return m_tail.cend();
}

Expression apply(const Atom & op, const std::vector<Expression>& args, const Environment& env) {

	// head must be a symbol
	if (!op.isSymbol()) {
		throw SemanticError("Error during evaluation: procedure name not symbol");
	}

	// must map to a proc
	if (!env.is_proc(op)) {
		throw SemanticError("Error during evaluation: symbol does not name a procedure");
	}

	// map from symbol to proc
	Procedure proc = env.get_proc(op);

	// call proc with args
	return proc(args);
}

Expression Expression::handle_lookup(const Atom& head, const Environment& env) {

	// if symbol is in env return value
	if (head.isSymbol()) {
		if(env.is_exp(head)) {
			return env.get_exp(head);
		} else {
			throw SemanticError("Error during evaluation: unknown symbol");
		}
	} else if (head.isNumber() || head.isComplex()) {
			return Expression(head);
	} else {
		throw SemanticError("Error during evaluation: Invalid type in terminal expression");
	}
}

Expression Expression::handle_begin(Environment& env) {
	if (m_tail.size() == 0) {
		throw SemanticError("Error during evaluation: zero arguments to begin");
	}

	// evaluate each arg from tail, return the last
	Expression result;
	for (Expression::IteratorType it = m_tail.begin(); it != m_tail.end(); ++it) {
		result = it->eval(env);
	}

	return result;
}


Expression Expression::handle_define(Environment& env) {

	// tail must have size 2 or error
	if (m_tail.size() != 2) {
		throw SemanticError("Error during evaluation: invalid number of arguments to define");
	}

	// tail[0] must be symbol
	if (!m_tail[0].isHeadSymbol()) {
		throw SemanticError("Error during evaluation: first argument to define not symbol");
	}

	// but tail[0] must not be a special-form or procedure
	Atom head = m_tail[0].head();
	if (head.asSymbol() == "define" || head.asSymbol() == "begin" || head == ListRoot || head == LambdaRoot) {
		throw SemanticError("Error during evaluation: attempt to redefine a special-form");
	}

	if (env.is_proc(m_tail[0].head())) {
		throw SemanticError("Error during evaluation: attempt to redefine a built-in procedure");
	}

	// eval tail[1]
	Expression result = m_tail[1].eval(env);

	if (env.is_exp(m_tail[0].head())) {
		throw SemanticError("Error during evaluation: attempt to redefine a previously defined "
			"symbol");
	}

	//and add to env
	env.add_exp(m_tail[0].head(), result);

	return result;
}

Expression Expression::handle_list(Environment& env) {
	std::vector<Expression> result;
	for (auto& a : m_tail) {
		result.push_back(a.eval(env));
	}

	return Expression(result);
}

Expression Expression::handle_lambda(Environment& env) {

	// Lambda needs a list of arguments and an expression to evaluate those arguments in
	if (m_tail.size() != 2) {
		throw SemanticError("Error during evaluation: invalid number of arguments to lambda");
	}

	// Copy this instance of the expression to avoid mutating itself
	Expression lambda(*this);

	// Reference the first element of the tail as the function arguments. The second expression
	// in the tail is the expression related to the lambda function itself. It gets
	// evaluated at run time
	Expression& lambdaArgs = lambda.m_tail[0];

	// Start evaluating the possible arguments for the lambda function. Start by moving the head to
	// the tail of the lambdaArgs expression
	lambdaArgs.m_tail.insert(lambdaArgs.tailConstBegin(), lambdaArgs.head());
	lambdaArgs.m_head = ListRoot;
	for (Expression& arg : lambdaArgs.m_tail) {

		// Need to ensure each argument is a symbol type expression that does not point to a procedure
		if (arg.isHeadSymbol()) {
			if (env.is_proc(arg.head())) {

				// Cannot use a built-in procedure as an argument (i.e. +, -, cos, etc.)
				throw SemanticError("Error during evaluation: procedures cannot be arguments for "
					"a lambda function");
			}
		} else if (arg.isHeadNumber()) {

			// Numbers cannot be arguments in a lambda function
			throw SemanticError("Error during evaluation: numbers cannot be arguments for "
				"a lambda function");
		}
	}

	// After processing the user's lambda function, the copied expression will contain the
	// lambda function's information. We just need to set the head to a lambda type
	lambda.m_head = LambdaRoot;
	return lambda;
}

// this is a simple recursive version. the iterative version is more
// difficult with the ast data structure used (no parent pointer).
// this limits the practical depth of our AST
Expression Expression::eval(Environment& env) {
	if (m_head.isSymbol() && m_head.asSymbol() == "begin") {

		// handle begin special-form
		return handle_begin(env);
	} else if (m_head.isSymbol() && m_head.asSymbol() == "define") {

		// handle define special-form
		return handle_define(env);
	} else if (isHeadListRoot()) {

		// handle list special-form
		return handle_list(env);
	} else if (isHeadLambdaRoot()) {

		// handle lambda special-form
		return handle_lambda(env);
	} else if (m_tail.empty()) {
		return handle_lookup(m_head, env);
	} else {

		// else attempt to treat as procedure
		std::vector<Expression> results;
		for (Expression::IteratorType it = m_tail.begin(); it != m_tail.end(); ++it) {
			results.push_back(it->eval(env));
		}

		return apply(m_head, results, env);
	}
}

std::ostream& operator<<(std::ostream& out, const Expression& exp) {
	out << "(";
	if (!exp.isHeadListRoot() && !exp.isHeadLambdaRoot()) {
		out << exp.head();
		if (exp.isHeadSymbol() && exp.tailConstBegin() != exp.tailConstEnd()) {

			// Symbols need to have a space after them (Symbols that have expressions in the tail
			// are procedures)
			out << " ";
		}
	}

	for (auto e = exp.tailConstBegin(); e != exp.tailConstEnd(); ++e) {
		if (e != exp.tailConstBegin()) {
			out << " ";
		}

		out << *e;
	}

	out << ")";
	return out;
}

bool Expression::operator==(const Expression& exp) const noexcept {
	bool result = (m_head == exp.m_head);

	result = result && (m_tail.size() == exp.m_tail.size());

	if (result) {
		for (auto lefte = m_tail.begin(), righte = exp.m_tail.begin();
			lefte != m_tail.end() && righte != exp.m_tail.end();
			++lefte, ++righte) {

			result = result && (*lefte == *righte);
		}
	}

	return result;
}

bool operator!=(const Expression& left, const Expression& right) noexcept {
	return !(left == right);
}
