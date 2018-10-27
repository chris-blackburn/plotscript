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
	m_tail = a.m_tail;

	// Deep copy the map from "a" and create a new unique pointer from it
	if (a.m_props.get() != nullptr) {
		m_props = std::unique_ptr<PropertyMap>(new PropertyMap(*a.m_props));
	}
}

Expression& Expression::operator=(const Expression& a) {

	// prevent self-assignment
	if (this != &a) {
		m_head = a.m_head;

		m_tail.clear();
		m_tail = a.m_tail;

		if (a.m_props.get() != nullptr) {
			m_props = std::unique_ptr<PropertyMap>(new PropertyMap(*a.m_props));
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

bool Expression::isHeadStringLiteral() const noexcept {
	return m_head.isStringLiteral();
}

bool Expression::isHeadListRoot() const noexcept {
	return m_head == ListRoot;
}

bool Expression::isHeadLambdaRoot() const noexcept {
	return m_head == LambdaRoot;
}

void Expression::setProperty(const std::string& key, const Expression& value) {

	// Construct a new property list if one doesn't already exist
	if (m_props.get() == nullptr) {
		m_props = std::unique_ptr<PropertyMap>(new PropertyMap);
	}

	// Add the key and the expression to the map
	(*m_props)[key] = value;
}

Expression Expression::getProperty(const std::string& property) const {
	if (m_props.get() != nullptr) {
		auto it = m_props->find(property);

		// if it was found, return the value
		if (it != m_props->cend()) {
			return it->second;
		}
	}

	// if no match was found, return an empty expression
	return Expression();
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

Expression apply_lambda(const Expression& lambda, const std::vector<Expression>& args,
	Environment env) {

	// Reference the arguments and expression of the lambda function. Only the expression needs
	// to be copied
	const Expression& lambdaArgs(*lambda.tailConstBegin());
	Expression lambdaExp(*std::prev(lambda.tailConstEnd()));

	// Get the iterators for the arguments of the lambda function
	auto lBegin = lambdaArgs.tailConstBegin();
	auto lEnd = lambdaArgs.tailConstEnd();

	// Make sure the number of argumentes between the lambda function and the passed in args match
	if (std::distance(lBegin, lEnd) == std::distance(args.cbegin(), args.cend())) {

		// loop through each argument and add it to the copied environment with the lambda arguments
		// as the symbols
		auto ut = args.cbegin();
		for (auto lt = lBegin; lt != lEnd; lt++, ut++) {

			// specify that we want to overwrite expressions in the environment
			env.add_exp(lt->head(), *ut, true);
		}

		// Evaluate the expression with the modified environment
		return lambdaExp.eval(env);
	} else {
		throw SemanticError("Error during evaluation: incorrect number of arguments to "
			"lambda function");
	}
}

Expression apply(const Atom & op, const std::vector<Expression>& args, const Environment& env) {

	// head must be a symbol
	if (!op.isSymbol()) {
		throw SemanticError("Error during evaluation: procedure name not symbol");
	}

	// must map to a proc
	if (!env.is_proc(op)) {

		// check if there is a lambda function in the environment
		Expression lambda = env.get_exp(op);
		if (lambda.isHeadLambdaRoot()) {
			return apply_lambda(lambda, args, env);
		} else {
			throw SemanticError("Error during evaluation: symbol does not name a procedure");
		}
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
	} else if (head.isNumber() || head.isComplex() || head.isStringLiteral()) {
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

bool Expression::isSpecialForm(const Atom& head) const {
	std::string s = head.asSymbol();
	return s == "define" ||
		s == "begin" ||
		s == "apply" ||
		s == "map" ||
		head == ListRoot ||
		head == LambdaRoot ||
		s == "set-property" ||
		s == "get-property";
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
	if (isSpecialForm(m_tail[0].head())) {
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

Expression Expression::handle_apply(Environment& env) {

	// The first expression is a procedure, and the second is the list of expressions
	if (m_tail.size() == 2) {

		// pre-evaluate the second expression to create a list
		Expression list = m_tail[1].eval(env);
		if (!list.isHeadListRoot()) {
			throw SemanticError("Error: second argument to apply not a list");
		}

		// create the list
		std::vector<Expression> applyArgs(list.tailConstBegin(), list.tailConstEnd());

		// to be a valid procedure, the expression should be JUST the procedure symbol
		Expression& proc = m_tail[0];
		if (proc.m_tail.empty() && env.is_proc(proc.head())) {
			return env.get_proc(proc.head())(applyArgs);

		// If the procedure is a pre-defined or anonymous lambda function
		} else {
			Expression lambda = proc.eval(env);

			// If we have a lambda function, evaluate the with that function
			if (lambda.isHeadLambdaRoot()) {
				return apply_lambda(lambda, applyArgs, env);
			}
		}

		throw SemanticError("Error: first argument to apply not a procedure");
	}

	throw SemanticError("Error: wrong number of arguments to apply which takes two arguments");
}

Expression Expression::handle_map(Environment& env) {

		// The first expression is a procedure, and the second is the list of expressions
		if (m_tail.size() == 2) {

			// pre-evaluate the second expression to create a list
			Expression list = m_tail[1].eval(env);
			if (!list.isHeadListRoot()) {
				throw SemanticError("Error: second argument to map not a list");
			}

			// create the list
			std::vector<Expression> mapList(list.tailConstBegin(), list.tailConstEnd());

			// to be a valid procedure, the expression should be JUST the procedure symbol
			Expression& proc = m_tail[0];
			if (proc.m_tail.empty() && env.is_proc(proc.head())) {
				for (Expression& a : mapList) {
					a = env.get_proc(proc.head())(std::vector<Expression>{a});
				}

				return Expression(mapList);

			// The procedure could be a pre-defined lambda or anonymous lambda
			} else {
				Expression lambda = proc.eval(env);

				// If we have a lambda function, evaluate the map with that function
				if (lambda.isHeadLambdaRoot()) {
					for (Expression& a : mapList) {
						a = apply_lambda(lambda, std::vector<Expression>{a}, env);
					}

					return Expression(mapList);
				}
			}

			throw SemanticError("Error: first argument to map not a procedure");
		}

		throw SemanticError("Error: wrong number of arguments to map which takes two arguments");
}

Expression Expression::handle_setProperty(Environment& env) {
	if (m_tail.size() == 3) {
		if (m_tail[0].isHeadStringLiteral()) {

			// If the expression already lives in the environment, we can modify it directly. Note, that
			// if the expression found is a lambda function with arguments, we only want to set a
			// property to its returned expression. Otherwise, if the lambda has no arguments, we can
			// set a property to the function itself
			Expression* expPtr = env.get_exp_ptr(m_tail[2].head());
			if (expPtr != nullptr && (!expPtr->isHeadLambdaRoot() ||
				(expPtr->isHeadLambdaRoot() && m_tail[2].m_tail.size() == 0))) {
				expPtr->setProperty(m_tail[0].head().asSymbol(true), m_tail[1].eval(env));
				return *expPtr;
			} else {

				// grab the evaluated expression to apply the property to
				Expression exp = m_tail[2].eval(env);
				exp.setProperty(m_tail[0].head().asSymbol(true), m_tail[1].eval(env));
				return exp;
			}
		}

		throw SemanticError("Error: first argument to set-property not a string literal");
	}

	throw SemanticError("Error: wrong number of arguments to set-property which takes three "
		"arguments");
}

Expression Expression::handle_getProperty(Environment& env) {
	if (m_tail.size() == 2) {
		if (m_tail[0].isHeadStringLiteral()) {

			// Get the expression from the environment or just evaluate it
			Expression exp = m_tail[1].eval(env);
			return exp.getProperty(m_tail[0].head().asSymbol(true));
		}

		throw SemanticError("Error: first argument to get-property not a string literal");
	}

	throw SemanticError("Error: wrong number of arguments to get-property which takes two "
		"arguments");
}

// this is a simple recursive version. the iterative version is more
// difficult with the ast data structure used (no parent pointer).
// this limits the practical depth of our AST
Expression Expression::eval(Environment& env) {
	if (m_head.asSymbol() == "begin") {

		// handle begin special-form
		return handle_begin(env);
	} else if (m_head.asSymbol() == "define") {

		// handle define special-form
		return handle_define(env);
	} else if (m_head.asSymbol() == "apply") {

		// handle apply special-form
		return handle_apply(env);
	} else if (m_head.asSymbol() == "map") {

		// handle map special-form
		return handle_map(env);
	} else if (isHeadListRoot()) {

		// handle list special-form
		return handle_list(env);
	} else if (isHeadLambdaRoot()) {

		// handle lambda special-form
		return handle_lambda(env);
	} else if (m_head.asSymbol() == "set-property") {

		// handle set-property special-form
		return handle_setProperty(env);
	} else if (m_head.asSymbol() == "get-property") {

		// handle get-property special-form
		return handle_getProperty(env);
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
	if (exp.head().isNone()) {
		out << "NONE";
		return out;
	}

	out << "(";
	if (!exp.isHeadListRoot() && !exp.isHeadLambdaRoot()) {
		out << exp.head();
		if (exp.isHeadSymbol() && exp.tailConstBegin() != exp.tailConstEnd()) {

			// Procedures need to have a space after them (Symbols that have expressions in the tail
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
