#include "environment.hpp"

#include <cassert>
#include <cmath>

#include "environment.hpp"
#include "semantic_error.hpp"

/***********************************************************************
Helper Functions
**********************************************************************/

// predicate, the number of args is nargs
bool nargs_equal(const std::vector<Expression>& args, unsigned nargs) {
	return args.size() == nargs;
}

/***********************************************************************
Each of the functions below have the signature that corresponds to the
typedef'd Procedure function pointer.
**********************************************************************/

// the default procedure always returns an expresison of type None
Expression default_proc(const std::vector<Expression>& args) {

	// make compiler happy we used this parameter
	args.size();
	return Expression();
};

Expression add(const std::vector<Expression>& args) {

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

Expression mul(const std::vector<Expression>& args) {

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

Expression subneg(const std::vector<Expression>& args) {
	complex result;
	bool isComplexProcedure = false;

	// If there is just one argument, we want to return the negative of that number
	if (nargs_equal(args, 1)) {
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

Expression div(const std::vector<Expression>& args) {
	complex result;
	bool isComplexProcedure = false;

	// If there is just one argument, we want to return the inverse of that number
	if (nargs_equal(args, 1)) {
		if (args[0].isHeadNumber()) {
			result = 1 / args[0].head().asNumber();
		} else if(args[0].isHeadComplex()) {
			isComplexProcedure = true;
			result = complex(1, 0) / args[0].head().asComplex();
		} else {
			throw SemanticError("Error in call to division: invalid argument.");
		}
	} else if (nargs_equal(args, 2)) {
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

Expression sqrt(const std::vector<Expression>& args) {
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

Expression pow(const std::vector<Expression>& args) {
	complex result;
	bool isComplexProcedure = false;

	// pow takes two arguments, a ^ b
	if (nargs_equal(args, 2)) {

		if (args[0].isHeadNumber() && args[1].isHeadNumber()) {
			result = std::pow(args[0].head().asNumber(), args[1].head().asNumber());
		} else if (args[0].isHeadComplex() || args[0].head().isNumber()) {
			isComplexProcedure = true;
			result = std::pow(args[0].head().asComplex(), args[1].head().asComplex());
		} else {
			throw SemanticError("Error in call to pow: invalid argument.");
		}
	} else {
		throw SemanticError("Error in call to pow: invalid number of arguments.");
	}

	return (isComplexProcedure) ? Expression(result) : Expression(result.real());
}

Expression ln(const std::vector<Expression>& args) {
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

Expression sin(const std::vector<Expression>& args) {
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

Expression cos(const std::vector<Expression>& args) {
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

Expression tan(const std::vector<Expression>& args) {
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

Expression real(const std::vector<Expression>& args) {
	double result = 0;

	if (nargs_equal(args, 1)) {
		if (args[0].isHeadComplex()) {
			result = args[0].head().asComplex().real();
		} else {
			throw SemanticError("Error in call to real: invalid argument.");
		}
	} else {
		throw SemanticError("Error in call to real: invalid number of arguments.");
	}

	return Expression(result);
}

Expression imag(const std::vector<Expression>& args) {
	double result = 0;

	if (nargs_equal(args, 1)) {
		if (args[0].isHeadComplex()) {
			result = args[0].head().asComplex().imag();
		} else {
			throw SemanticError("Error in call to imag: invalid argument.");
		}
	} else {
		throw SemanticError("Error in call to imag: invalid number of arguments.");
	}

	return Expression(result);
}

Expression mag(const std::vector<Expression>& args) {
	double result = 0;

	if (nargs_equal(args, 1)) {
		if (args[0].isHeadComplex()) {
			result = std::abs(args[0].head().asComplex());
		} else {
			throw SemanticError("Error in call to mag: invalid argument.");
		}
	} else {
		throw SemanticError("Error in call to mag: invalid number of arguments.");
	}

	return Expression(result);
}

Expression arg(const std::vector<Expression>& args) {
	double result = 0;

	if (nargs_equal(args, 1)) {
		if (args[0].isHeadComplex()) {
			result = std::arg(args[0].head().asComplex());
		} else {
			throw SemanticError("Error in call to arg: invalid argument.");
		}
	} else {
		throw SemanticError("Error in call to arg: invalid number of arguments.");
	}

	return Expression(result);
}

Expression conj(const std::vector<Expression>& args) {
	complex result = 0;

	if (nargs_equal(args, 1)) {
		if (args[0].isHeadComplex()) {
			result = std::conj(args[0].head().asComplex());
		} else {
			throw SemanticError("Error in call to conj: invalid argument.");
		}
	} else {
		throw SemanticError("Error in call to conj: invalid number of arguments.");
	}

	return Expression(result);
}

// ******** List related functions ********
Expression first(const std::vector<Expression>& args) {
	if (nargs_equal(args, 1)) {
		if (args[0].isHeadListRoot()) {
			if (args[0].tailConstBegin() != args[0].tailConstEnd()) {
				return Expression(*args[0].tailConstBegin());
			}

			// When the iterators for beginnging and end are equal,the list is empty
			throw SemanticError("Error: argument to first is an empty list");
		}

		// if there is one argument that is not a list,
		throw SemanticError("Error: argument to first is not a list");
	}

	// This will only get triggered when there is more than one argument pass to first
	throw SemanticError("Error: more than one argument in call to first");
}

Expression rest(const std::vector<Expression>& args) {
	if (nargs_equal(args, 1)) {
		if (args[0].isHeadListRoot()) {
			std::vector<Expression>::const_iterator cbegin = args[0].tailConstBegin();
			std::vector<Expression>::const_iterator cend = args[0].tailConstEnd();
			if (cbegin != cend) {
				std::vector<Expression> result(cbegin + 1, cend);
				return Expression(result);
			}

			// When the iterators for beginnging and end are equal,the list is empty
			throw SemanticError("Error: argument to rest is an empty list");
		}

		// if there is one argument that is not a list,
		throw SemanticError("Error: argument to rest is not a list");
	}

	// This will only get triggered when there is more than one argument that passes
	throw SemanticError("Error: more than one argument in call to rest");
}

Expression length(const std::vector<Expression>& args) {
	if (nargs_equal(args, 1)) {
		if (args[0].isHeadListRoot()) {
			std::vector<Expression>::const_iterator cbegin = args[0].tailConstBegin();
			std::vector<Expression>::const_iterator cend = args[0].tailConstEnd();
			return Expression(std::distance(cbegin, cend));
		}

		// if there is one argument that is not a list,
		throw SemanticError("Error: argument to length is not a list");
	}

	// This will only get triggered when there is more than one argument that passes
	throw SemanticError("Error: more than one argument in call to length");
}

Expression append(const std::vector<Expression>& args) {
	if (nargs_equal(args, 2)) {
		if (args[0].isHeadListRoot()) {
			std::vector<Expression>::const_iterator cbegin = args[0].tailConstBegin();
			std::vector<Expression>::const_iterator cend = args[0].tailConstEnd();

			// Copy the list and add the second argument to the new list
			std::vector<Expression> result(cbegin, cend);
			result.push_back(args[1]);
			return Expression(result);
		}

		// if there is one argument that is not a list,
		throw SemanticError("Error: the first argument to append should be a list");
	}

	// This will only get triggered when there is the wrong number of arguments
	throw SemanticError("Error: wrong number of arguments for append which takes two arguments");
}

Expression join(const std::vector<Expression>& args) {
	if (nargs_equal(args, 2)) {
		if (args[0].isHeadListRoot() && args[1].isHeadListRoot()) {

			// Iterators for first list
			std::vector<Expression>::const_iterator cbegin1 = args[0].tailConstBegin();
			std::vector<Expression>::const_iterator cend1 = args[0].tailConstEnd();

			// iterators for second list
			std::vector<Expression>::const_iterator cbegin2 = args[1].tailConstBegin();
			std::vector<Expression>::const_iterator cend2 = args[1].tailConstEnd();

			// preallocate space in the new result vector
			std::vector<Expression> result;
			result.reserve(std::distance(cbegin1, cend1) + std::distance(cbegin2, cend2));

			// Add the elements from the first list then from the second list to the result vector
			result.insert(result.end(), cbegin1, cend1);
			result.insert(result.end(), cbegin2, cend2);

			return Expression(result);
		}

		// if there is one argument that is not a list,
		throw SemanticError("Error: one of the arguments to join is not a list");
	}

	// This will only get triggered when there is the wrong number of arguments
	throw SemanticError("Error: wrong number of arguments for join which takes two arguments");
}

Expression range(const std::vector<Expression>& args) {
	if (nargs_equal(args, 3)) {
		if (args[0].isHeadNumber() && args[1].isHeadNumber() && args[2].isHeadNumber()) {
			if (args[1].head().asNumber() > args[0].head().asNumber()) {
				if (args[2].head().asNumber() > 0) {
					double begin = args[0].head().asNumber();
					double end = args[1].head().asNumber();
					double step = args[2].head().asNumber();

					// pre-allocate memory into the results vector
					std::vector<Expression> result;
					result.reserve(((end - begin) / step));

					// Actually perform the count and save to result
					for (double i = begin; i <= end; i = i + step) {
						result.push_back(Expression(i));
					}

					return Expression(result);
				}

				// The step argument is not positive
				throw SemanticError("Error: negative or zero increment in range");
			}

			// The second number in the range is larger than the first
			throw SemanticError("Error: end should be greater than begin in range");
		}

		// if there is one argument that is not a number,
		throw SemanticError("Error: one of the arguments to range is not a number");
	}

	// This will only get triggered when there is the wrong number of arguments
	throw SemanticError("Error: wrong number of arguments for range which takes three arguments");
}

// **************** Plotting procedures ****************
// Spacing constants
#define PlotN 20
#define PlotA 3
#define PlotB 3
#define PlotC 2
#define PlotD 2
#define PlotP 0.5

// convenience struct to hold the bounds values
typedef struct _Bounds {
	double AL, AU, OL, OU;
} Bounds;

// convenience struct for lines and points
typedef struct _Point {
	double x, y;
} Point;

typedef struct _Line {
	double x1, x2, y1, y2;
} Line;

// Helper function to create a plotscript point object
Expression makePointExpression(Point p, double size = 0) {

	// NOTE: Ordinate values are negated because of Qt's coordinate system
	Expression point({Expression(p.x), Expression(-p.y)});
	point.setProperty("object-name", Expression(Atom("point")));
	point.setProperty("size", Expression(size));
	return point;
}

// Helper function to create a plotscript line object
Expression makeLineExpression(Line l) {
	Expression line({makePointExpression({l.x1, l.y1}), makePointExpression({l.x2, l.y2})});
	line.setProperty("object-name", Expression(Atom("line")));
	line.setProperty("thickness", Expression(0));
	return line;
}

// Helper function to handle any possible options for plots (title, axis labels, etc.)
Expression handlePlotOptions(const Expression& options) {

	// the options expression should be a list
	if (options.isHeadListRoot()) {

		// TODO: Check for known options and verify they are of the correct type
		return options;
	}

	throw SemanticError("Error: options for plot is not a list");
}

// returns an std::pair of two doubles (convenience when working with point lists)
Point getPointValues(const Expression& point) {
	auto cbegin = point.tailConstBegin();
	auto cend = point.tailConstEnd();

	// If cbegin + 2 is the end, then that means only two elements were in the point list
	if (cbegin + 2 == cend) {
		const Expression x = *cbegin;
		const Expression y = *(cbegin + 1);

		if (x.isHeadNumber() && y.isHeadNumber()) {
			return {x.head().asNumber(), y.head().asNumber()};
		}

		throw SemanticError("Error: NaN or complex value for point in plot");
	}

	throw SemanticError("Error: not a valid point for plot");
}

// returns an vector of the AL, AU, OL, and OU values in that order
Bounds getBoundsFromList(const Expression& data) {

	// data should be a list expression when this function is called
	auto dataBegin = data.tailConstBegin();
	auto dataEnd = data.tailConstEnd();

	// If there are no points or just one point, then throw an exception
	if (dataBegin == dataEnd || dataBegin + 1 == dataEnd) {
		throw SemanticError("Error: not enough data points for plot");
	}

	// prime the bound values (grab the first point and set the bounds)
	Point firstPoint = getPointValues(*dataBegin);
	double AL = firstPoint.x, AU = AL,
		OL = firstPoint.y, OU = OL;

	// Traverse through the list of points to find the minima and maxima
	// I increment dataBegin before assigning it to "it" because we already got the first point
	for (auto it = dataBegin + 1; it != dataEnd; it++) {
		Point p = getPointValues(*it);

		// update the bound values
		if (AL > p.x) {
			AL = p.x;
		} else if (AU < p.x) {
			AU = p.x;
		}

		if (OL > p.y) {
			OL = p.y;
		} else if (OU < p.y) {
			OU = p.y;
		}
	}

	return {AL, AU, OL, OU};
}

// Helper function that creates the list of line and point objects to represent the data
Expression formatDataDiscrete(const Expression& data, const Bounds& bounds,
	double aScaleFactor, double oScaleFactor) {
	std::vector<Expression> result;

	// First, we need to build the edges. start by getting the scaled bound values
	Bounds scaledB = {bounds.AL * aScaleFactor, bounds.AU * aScaleFactor,
		bounds.OL * oScaleFactor, bounds.OU * oScaleFactor};

	// Axis creation. For the abscissa, we need a number to create the stems from (absOrigin will
	// serve as the ordinate axis value for where the line starts from)
	double absOrigin = 0;
	if (0 > scaledB.OL && 0 > scaledB.OU) {
		absOrigin = scaledB.OU;
	} else if (0 < scaledB.OL && 0 < scaledB.OU) {
		absOrigin = scaledB.OL;
	} else {

		// If the origin is in the plot (absOrigin == 0), then we need to create another axis line
		result.push_back(makeLineExpression({scaledB.AL, scaledB.AU, 0, 0}));
	}

	// For the ordinate axis, we can go ahead and create the line if needed
	if (0 > scaledB.AL && 0 < scaledB.AU) {
		result.push_back(makeLineExpression({0, 0, scaledB.OL, scaledB.OU}));
	}

	// Data bounding box edges (top, bottom, left, right)
	result.push_back(makeLineExpression({scaledB.AL, scaledB.AU, scaledB.OU, scaledB.OU}));
	result.push_back(makeLineExpression({scaledB.AL, scaledB.AU, scaledB.OL, scaledB.OL}));
	result.push_back(makeLineExpression({scaledB.AL, scaledB.AL, scaledB.OU, scaledB.OL}));
	result.push_back(makeLineExpression({scaledB.AU, scaledB.AU, scaledB.OU, scaledB.OL}));

	// Now that we have all the bounding information, we can create all the lines and points
	// that consitute the data in the plot
	// TODO: for each point in the list of data points, scale and create the line and point objects

	// TODO: Create the text objects for the plot itself

	// TODO: remove
	data.head().asNumber();
	Expression dummy(absOrigin);
	return Expression(result);
}

Expression discretePlot(const std::vector<Expression>& args) {

	// Discrete plots can take one or two arguments (options are, optional)
	bool justData = nargs_equal(args, 1);
	bool dataAndOptions = nargs_equal(args, 2);

	if (justData || dataAndOptions) {

		// Create a reference to the data. Options will get handled later if they exist
		const Expression& data = args[0];

		// The data should be a list. Start processing the plot data
		if (data.isHeadListRoot()) {

			// First, we need to find the upper and lower bounds for each axis. Then we can determine
			// the scale factor as well as the positioning for each object in the plot
			Bounds b = getBoundsFromList(data);

			// Determine the scale factor based off the minima and maxima of the plot
			// TODO: If the two bounds are the same value, span from the axis to that value
			double aScaleFactor = PlotN / (b.AU - b.AL);
			double oScaleFactor = PlotN / (b.OU - b.OL);

			// Create the list of line and point objects using the data and scale factors
			Expression formattedData = formatDataDiscrete(data, b, aScaleFactor, oScaleFactor);

			// 		TODO: for the below tasks, pass in the amin and omin values for positioning
			// TODO: Create the AL, AU, OL, OU text objects (precision of 2 decimal places)
			// TODO: Create the title, abscissa label, ordinate label text objects

			// return the scaled data and options
			// TODO: also return AL, AU, OL, OU after the data and before the options
			if (dataAndOptions) {
				return handlePlotOptions(args[1]);
				// return join({scaledData, handlePlotOptions(args[1])});
			}

			// if no options exist, just return the data
			return formattedData;
		}

		// Both arguements should be a list
		throw SemanticError("Error: arguements to discrete-plot should be lists");
	}

	// This will only get triggered when there is the wrong number of arguments
	throw SemanticError("Error: wrong number of arguments for discrete-plot which "
		"takes one or two arguments");
}

const double PI = std::atan2(0, -1);
const double EXP = std::exp(1);
const complex I = complex(0, 1);

Environment::Environment() {
	reset();
}

bool Environment::is_known(const Atom& sym) const {
	if (!sym.isSymbol()) {
		return false;
	}

	return envmap.find(sym.asSymbol()) != envmap.end();
}

bool Environment::is_exp(const Atom& sym) const {
	if(!sym.isSymbol()) {
		return false;
	}

	auto result = envmap.find(sym.asSymbol());
	return (result != envmap.end()) && (result->second.type == ExpressionType);
}

Expression Environment::get_exp(const Atom& sym) const {
	Expression exp;

	if (sym.isSymbol()) {
		auto result = envmap.find(sym.asSymbol());
		if (result != envmap.end() && result->second.type == ExpressionType) {
			exp = result->second.exp;
		}
	}

	return exp;
}

Expression* Environment::get_exp_ptr(const Atom& sym) {
	if (sym.isSymbol()) {
		auto result = envmap.find(sym.asSymbol());
		if (result != envmap.end() && result->second.type == ExpressionType) {
			return &result->second.exp;
		}
	}

	// if the expression isn't found in the map, then return a null pointer
	return nullptr;
}

void Environment::add_exp(const Atom& sym, const Expression& exp, bool overwrite) {
	if (!sym.isSymbol()) {
		throw SemanticError("Attempt to add non-symbol to environment");
	}

	// error if overwriting symbol map unless overwrite flag is true
	if (!overwrite && envmap.find(sym.asSymbol()) != envmap.end()) {
		throw SemanticError("Attempt to overwrite symbol in environemnt");
	}

	envmap[sym.asSymbol()] = EnvResult(ExpressionType, exp);
}

bool Environment::is_proc(const Atom& sym) const {
	if (!sym.isSymbol()) {
		return false;
	}

	auto result = envmap.find(sym.asSymbol());
	return (result != envmap.end()) && (result->second.type == ProcedureType);
}

Procedure Environment::get_proc(const Atom& sym) const {
	if (sym.isSymbol()) {
		auto result = envmap.find(sym.asSymbol());
		if (result != envmap.end() && result->second.type == ProcedureType) {
			return result->second.proc;
		}
	}

	return default_proc;
}

/*
Reset the environment to the default state. First remove all entries and
then re-add the default ones.
 */
void Environment::reset() {
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

	// Procedure: real
	envmap.emplace("real", EnvResult(ProcedureType, real));

	// Procedure: imag
	envmap.emplace("imag", EnvResult(ProcedureType, imag));

	// Procedure: mag
	envmap.emplace("mag", EnvResult(ProcedureType, mag));

	// Procedure: arg
	envmap.emplace("arg", EnvResult(ProcedureType, arg));

	// Procedure: conj
	envmap.emplace("conj", EnvResult(ProcedureType, conj));

	// Procedure: first
	envmap.emplace("first", EnvResult(ProcedureType, first));

	// Procedure: rest
	envmap.emplace("rest", EnvResult(ProcedureType, rest));

	// Procedure: length
	envmap.emplace("length", EnvResult(ProcedureType, length));

	// Procedure: append
	envmap.emplace("append", EnvResult(ProcedureType, append));

	// Procedure: join
	envmap.emplace("join", EnvResult(ProcedureType, join));

	// Procedure: range
	envmap.emplace("range", EnvResult(ProcedureType, range));

	// Procedure: discrete-plot
	envmap.emplace("discrete-plot", EnvResult(ProcedureType, discretePlot));
}
