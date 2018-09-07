#include "environment.hpp"

#include <cassert>
#include <cmath>

#include "environment.hpp"
#include "semantic_error.hpp"

/***********************************************************************
Helper Functions
**********************************************************************/

// predicate, the number of args is nargs
bool nargs_equal(const std::vector<Expression> & args, unsigned nargs) {
	return args.size() == nargs;
}

/***********************************************************************
Each of the functions below have the signature that corresponds to the
typedef'd Procedure function pointer.
**********************************************************************/

// the default procedure always returns an expresison of type None
Expression default_proc(const std::vector<Expression> & args) {
	args.size(); // make compiler happy we used this parameter
	return Expression();
};

Expression add(const std::vector<Expression> & args) {

	// check all aruments are numbers or complex, while adding
	// I set the result to be complex and return the real value if no complex
	// numbers were in the expressions
	complex result = complex(0, 0);
	bool isComplexProcedure = false;

	for (auto& a : args) {
		if (a.isHeadNumber()) {
			result += a.head().asNumber();
		} else if (a.isHeadComplex()) {
			isComplexProcedure = true;
			result += a.head().asComplex();
		} else {
			throw SemanticError("Error in call to add, argument not a (complex) number");
		}
	}

	return (isComplexProcedure) ? Expression(result) : Expression(result.real());
};

Expression mul(const std::vector<Expression> & args){

	// The complex result needs to be initialized to (1, 0) for normal multiplication
	// to occur. The complex number class will handle incorperating complex numbers.
	complex result = complex(1, 0);
	bool isComplexProcedure = false;

	for (auto& a : args) {
		if (a.isHeadNumber()) {
			result *= a.head().asNumber();
		} else if (a.isHeadComplex()) {
			isComplexProcedure = true;
			result *= a.head().asComplex();
		} else{
			throw SemanticError("Error in call to mul, argument not a number");
		}
	}

	return (isComplexProcedure) ? Expression(result) : Expression(result.real());
};

Expression subneg(const std::vector<Expression> & args){

	complex result;
	bool isComplexProcedure = false;

	// If there is just one argument, we want to return the negative of that number
	if(nargs_equal(args, 1)){
		if (args[0].isHeadNumber()) {
			result = -args[0].head().asNumber();
		} else if(args[0].isHeadComplex()) {
			isComplexProcedure = true;
			result = -args[0].head().asComplex();
		} else {
			throw SemanticError("Error in call to negate: invalid argument.");
		}
	} else if (nargs_equal(args, 2)) {

		// Either both are numbers, one or both are complex, or niether are numbers or complex
		if (args[0].isHeadNumber() && args[1].isHeadNumber()) {
			result = args[0].head().asNumber() - args[1].head().asNumber();
		} else if (args[0].isHeadComplex() || args[1].isHeadComplex()) {
			isComplexProcedure = true;

			// when either number is complex, we can retrieve the atom as complex
			result = args[0].head().asComplex() - args[1].head().asComplex();
		} else {
			throw SemanticError("Error in call to subtraction: invalid argument.");
		}
	} else {
		throw SemanticError("Error in call to subtraction or negation: invalid number of arguments.");
	}

	return (isComplexProcedure) ? Expression(result) : Expression(result.real());
};

Expression div(const std::vector<Expression> & args){

	complex result;
	bool isComplexProcedure = false;

	if (nargs_equal(args, 2)) {
		if (args[0].isHeadNumber() && args[1].isHeadNumber()) {
			result = args[0].head().asNumber() / args[1].head().asNumber();
		} else if (args[0].isHeadComplex() || args[1].isHeadComplex()) {
			isComplexProcedure = true;

			// When either argument is complex, this becomes a complex operation
			result = args[0].head().asComplex() / args[1].head().asComplex();
		} else {
			throw SemanticError("Error in call to division: invalid argument.");
		}
	} else {
		throw SemanticError("Error in call to division: invalid number of arguments.");
	}

	return (isComplexProcedure) ? Expression(result) : Expression(result.real());
};

Expression sqrt(const std::vector<Expression> & args) {
	complex result;
	bool isComplexProcedure = false;

	// Make sure only one argument is present
	if (nargs_equal(args, 1)) {

		// If the atom is a number greater than or equal to zero, then proceed as normal.
		// If not, we need to check if it's complex, or still a number (implicitly negative),
		// and calculate with complex sqrt.
		if (args[0].head().isNumber() && args[0].head().asNumber() >= 0) {
			result = std::sqrt(args[0].head().asNumber());
		} else if (args[0].isHeadComplex() || args[0].head().isNumber()) {
			isComplexProcedure = true;
			result = std::sqrt(args[0].head().asComplex());
		} else {
			throw SemanticError("Error in call to square root: invalid argument.");
		}
	} else {
		throw SemanticError("Error in call to square root: invalid number of arguments.");
	}

	return (isComplexProcedure) ? Expression(result) : Expression(result.real());
}

Expression pow(const std::vector<Expression> & args) {
	double result = 0;

	// pow takes two arguments, a ^ b
	if (nargs_equal(args, 2)) {
		if (args[0].isHeadNumber() && args[1].isHeadNumber()) {
				result = std::pow(args[0].head().asNumber(), args[1].head().asNumber());
		} else {
			throw SemanticError("Error in call to pow: invalid argument.");
		}
	} else {
		throw SemanticError("Error in call to pow: invalid number of arguments.");
	}

	return Expression(result);
}

Expression ln(const std::vector<Expression> & args) {
	double result = 0;

	// ln takes one argument
	if (nargs_equal(args, 1)) {
		if (args[0].isHeadNumber()) {

			// Must be greater than or equal to zero. ln(0) returns -Inf
			if (args[0].head().asNumber() >= 0) {

				// In the standard lib, log denotes natural logarithm.
				result = std::log(args[0].head().asNumber());
			} else {
				throw SemanticError("Error in call to natural log: cannot take the natural log"
					" of a negative number.");
			}
		} else {
			throw SemanticError("Error in call to natural log: invalid argument.");
		}
	} else {
		throw SemanticError("Error in call to natural log: invalid number of arguments.");
	}

	return Expression(result);
}

Expression sin(const std::vector<Expression> & args) {
	double result = 0;

	// sin takes one argument
	if (nargs_equal(args, 1)) {
		if (args[0].isHeadNumber()) {
			result = std::sin(args[0].head().asNumber());
		} else {
			throw SemanticError("Error in call to sin: invalid argument.");
		}
	} else {
		throw SemanticError("Error in call to sin: invalid number of arguments.");
	}

	return Expression(result);
}

Expression cos(const std::vector<Expression> & args) {
	double result = 0;

	// cos takes one argument
	if (nargs_equal(args, 1)) {
		if (args[0].isHeadNumber()) {
			result = std::cos(args[0].head().asNumber());
		} else {
			throw SemanticError("Error in call to cos: invalid argument.");
		}
	} else {
		throw SemanticError("Error in call to cos: invalid number of arguments.");
	}

	return Expression(result);
}

Expression tan(const std::vector<Expression> & args) {
	double result = 0;

	// tan takes one argument
	if (nargs_equal(args, 1)) {
		if (args[0].isHeadNumber()) {
			result = std::tan(args[0].head().asNumber());
		} else {
			throw SemanticError("Error in call to tan: invalid argument.");
		}
	} else {
		throw SemanticError("Error in call to tan: invalid number of arguments.");
	}

	return Expression(result);
}

const double PI = std::atan2(0, -1);
const double EXP = std::exp(1);
const complex I = complex(0, 1);

Environment::Environment(){

	reset();
}

bool Environment::is_known(const Atom & sym) const{
	if(!sym.isSymbol()) return false;

	return envmap.find(sym.asSymbol()) != envmap.end();
}

bool Environment::is_exp(const Atom & sym) const{
	if(!sym.isSymbol()) return false;

	auto result = envmap.find(sym.asSymbol());
	return (result != envmap.end()) && (result->second.type == ExpressionType);
}

Expression Environment::get_exp(const Atom & sym) const{

	Expression exp;

	if(sym.isSymbol()){
		auto result = envmap.find(sym.asSymbol());
		if((result != envmap.end()) && (result->second.type == ExpressionType)){
			exp = result->second.exp;
		}
	}

	return exp;
}

void Environment::add_exp(const Atom & sym, const Expression & exp){

	if(!sym.isSymbol()){
		throw SemanticError("Attempt to add non-symbol to environment");
	}

	// error if overwriting symbol map
	if(envmap.find(sym.asSymbol()) != envmap.end()){
		throw SemanticError("Attempt to overwrite symbol in environemnt");
	}

	envmap.emplace(sym.asSymbol(), EnvResult(ExpressionType, exp));
}

bool Environment::is_proc(const Atom & sym) const{
	if(!sym.isSymbol()) return false;

	auto result = envmap.find(sym.asSymbol());
	return (result != envmap.end()) && (result->second.type == ProcedureType);
}

Procedure Environment::get_proc(const Atom & sym) const{

	//Procedure proc = default_proc;

	if(sym.isSymbol()){
		auto result = envmap.find(sym.asSymbol());
		if((result != envmap.end()) && (result->second.type == ProcedureType)){
			return result->second.proc;
		}
	}

	return default_proc;
}

/*
Reset the environment to the default state. First remove all entries and
then re-add the default ones.
 */
void Environment::reset(){

	envmap.clear();

	// Built-In value of pi
	envmap.emplace("pi", EnvResult(ExpressionType, Expression(PI)));

	// Built_In value of euler's number
	envmap.emplace("e", EnvResult(ExpressionType, Expression(EXP)));

	// Built_In value of the imaginary number
	envmap.emplace("I", EnvResult(ExpressionType, Expression(I)));

	// Procedure: add;
	envmap.emplace("+", EnvResult(ProcedureType, add));

	// Procedure: subneg;
	envmap.emplace("-", EnvResult(ProcedureType, subneg));

	// Procedure: mul;
	envmap.emplace("*", EnvResult(ProcedureType, mul));

	// Procedure: div;
	envmap.emplace("/", EnvResult(ProcedureType, div));

	// Procedure: sqrt
	envmap.emplace("sqrt", EnvResult(ProcedureType, sqrt));

	// Procedure: pow
	envmap.emplace("^", EnvResult(ProcedureType, pow));

	// Procedure: ln
	envmap.emplace("ln", EnvResult(ProcedureType, ln));

	// Procedure: sin
	envmap.emplace("sin", EnvResult(ProcedureType, sin));

	// Procedure: cos
	envmap.emplace("cos", EnvResult(ProcedureType, cos));

	// Procedure: tan
	envmap.emplace("tan", EnvResult(ProcedureType, tan));
}
