#include "expression.hpp"

#include <sstream>
#include <iomanip>
#include <cmath>
#include <limits>

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

		// Clear out the contents of this property map if it exists. This needs to happen regardless
		// of the state of the right hand side's property map
		if (m_props != nullptr) {
			m_props->clear();
		}

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

// Forward declare the plot functions for apply
Expression discretePlot(const std::vector<Expression>& args);
Expression continuousPlot(const std::vector<Expression>& args, const Environment& env);

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

			// Check if it is one of the plot commands
		} else if (op.asSymbol() == "discrete-plot") {
			return discretePlot(args);
		} else if (op.asSymbol() == "continuous-plot") {
			return continuousPlot(args, env);
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
		s == "get-property" ||
		s == "discrete-plot" ||
		s == "continuous-plot";
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

Expression Expression::evalLambda(const std::vector<Expression>& input,
		const Environment& env) const {
	if (isHeadLambdaRoot()) {
		return apply_lambda(*this, input, env);
	}

	return Expression();
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

// **************** Plotting procedures ****************
const double PI = std::atan2(0, -1);

// Spacing constants
#define PLOT_N 20
#define PLOT_A 3
#define PLOT_B 3
#define PLOT_C 2
#define PLOT_D 2
#define PLOT_P 0.5
#define PLOT_M 50
#define PLOT_LINE_ANG_MIN 175
#define PLOT_SPLIT_MAX 10

// convenience struct to hold the bounds values and stem abscissa starting value
typedef struct _Bounds {
	double AL, AU, OL, OU;

	// Calculate the scale factors for the bounds of this object
	double calcAbsScale() const {
		return PLOT_N / (AU - AL);
	}

	double calcOrdScale() const {
		return PLOT_N / (OU - OL);
	}

	// Helper function that returns the values scaled for the Qt notebook
	_Bounds scaleForGraphics() const {

		// TODO: If the two bounds are the same value, span from the axis to that value
		double absScale = calcAbsScale();
		double ordScale = calcOrdScale();

		return {AL * absScale, AU * absScale,
			OL * ordScale, OU * ordScale};
	}
} Bounds;

// convenience struct for lines and points
typedef struct _Point {
	double x, y;

	_Point scaleForGraphics(double absScaleFactor, double ordScaleFactor) const {
		return {x * absScaleFactor, y * ordScaleFactor};
	}
} Point;

typedef struct _Line {
	double x1, x2, y1, y2;

	_Line scaleForGraphics(double absScaleFactor, double ordScaleFactor) const {
		return {x1 * absScaleFactor, x2 * absScaleFactor,
			y1 * ordScaleFactor, y2 * ordScaleFactor};
	}
} Line;

double dX(const Line& l) {
	return l.x2 - l.x1;
}

double dY(const Line& l) {
	return l.y2 - l.y1;
}

double slope(const Line& l) {
	return dY(l) / dX(l);
}

double angleToXAxis(const Line& l) {
	return std::abs(std::atan(slope(l)) * (180 / PI));
}

// Helper function to create a plotscript point object
Expression makePointExpression(Point p, double size = 0) {

	// NOTE: Ordinate values are negated because of Qt's coordinate system
	Expression point({Expression(p.x), Expression(-p.y)});
	point.setProperty("object-name", Expression(Atom("\"point\"")));
	point.setProperty("size", Expression(size));
	return point;
}

// Helper function to create a plotscript line object
Expression makeLineExpression(Line l) {
	Expression line({makePointExpression({l.x1, l.y1}), makePointExpression({l.x2, l.y2})});
	line.setProperty("object-name", Expression(Atom("\"line\"")));
	line.setProperty("thickness", Expression(0));
	return line;
}

// Helper function to create a plotscript text object
Expression makeTextExpression(const std::string& text, Point point, double scale = 1,
	double rotation = 0) {
	Expression textExp(Atom('"' + text + '"'));
	textExp.setProperty("object-name", Expression(Atom("\"text\"")));
	textExp.setProperty("position", makePointExpression(point));
	textExp.setProperty("text-scale", Expression(scale));
	textExp.setProperty("text-rotation", Expression(rotation));
	return textExp;
}

// Helper function to get a key-value pair from the plot options
std::pair<const Expression&, const Expression&> getOptionKeyValue(const Expression& option) {
	if (option.isHeadListRoot()) {
		auto it = option.tailConstBegin();

		// If option + 2 is the end, then there were only two things in the option list, ignore
		// option lists that have incorrect or invalid keys/values.
		if (it + 2 == option.tailConstEnd()) {

			// return the key, value pair
			return {*it, *(it + 1)};
		}
	}

	return {Expression(), Expression()};
}

// convenience struct to house the possible plot options
typedef struct _PlotOptions {
	std::string title, abscissaLabel, ordinateLabel;
	double textScale = 1;
} PlotOptions;

// Helper function to apply plot options
void applyPlotOptions(std::vector<Expression>& plotData, const PlotOptions& options,
	const Bounds& scaled) {
	if (options.title != std::string()) {
		plotData.push_back(makeTextExpression(options.title,
			{scaled.AL + (PLOT_N / 2), scaled.OU + PLOT_A}, options.textScale));
	}

	if (options.abscissaLabel != std::string()) {
		plotData.push_back(makeTextExpression(options.abscissaLabel,
			{scaled.AL + (PLOT_N / 2), scaled.OL - PLOT_A}, options.textScale));
	}

	if (options.ordinateLabel != std::string()) {
		plotData.push_back(makeTextExpression(options.ordinateLabel,
			{scaled.AL - PLOT_B, scaled.OL + (PLOT_N / 2)}, options.textScale, -PI / 2));
	}
}

// Helper function to handle any possible options for plots (title, axis labels, etc.)
// it returns the text scale if it was set (1 if it was not set)
double handlePlotOptions(std::vector<Expression>& plotData, const Expression& options,
	const Bounds& scaled) {

	// the options expression should be a list
	if (options.isHeadListRoot()) {
		PlotOptions plotOptions;

		auto optionsBegin = options.tailConstBegin();
		auto optionsEnd = options.tailConstEnd();

		// Check for known options and verify they are of the correct type.
		for (auto it = optionsBegin; it != optionsEnd; it++) {
			std::pair<const Expression&, const Expression&> option = getOptionKeyValue(*it);

			// If there is a title,
			if (option.first.head().asSymbol(true) == "title") {
				if (option.second.isHeadStringLiteral()) {
					plotOptions.title = option.second.head().asSymbol(true);
				}

				// If there is an abscissa label,
			} else if (option.first.head().asSymbol(true) == "abscissa-label") {
				if (option.second.isHeadStringLiteral()) {
					plotOptions.abscissaLabel = option.second.head().asSymbol(true);
				}

				// If there is an ordinate label,
			} else if (option.first.head().asSymbol(true) == "ordinate-label") {
				if (option.second.isHeadStringLiteral()) {
					plotOptions.ordinateLabel = option.second.head().asSymbol(true);
				}

				// If there is a text-scale option,
			} else if (option.first.head().asSymbol(true) == "text-scale") {
				if (option.second.isHeadNumber()) {
					plotOptions.textScale = option.second.head().asNumber();
				}
			}
		}

		// After all options have been evaluated, apply them and return the text scale
		applyPlotOptions(plotData, plotOptions, scaled);
		return plotOptions.textScale;
	}

	throw SemanticError("Error: options for plot is not a list");
}

// returns Point object (convenience when working with point lists)
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

// returns a bounds object (AL, AU, OL, and OU) based off a list of points
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

// Helper function that adds the abscissa and ordinate axes to a vector of expressions. Also
// returns the starting point for stem plot lines
double addPlotAxes(std::vector<Expression>& plotData, const Bounds& scaled) {

	// Adding the ordinate axis
	if (0 > scaled.AL && 0 < scaled.AU) {
		plotData.push_back(makeLineExpression({0, 0, scaled.OL, scaled.OU}));
	}

	// In the cases where the abscissa axis does not need to be created, we still need a reference
	// point to stem the discrete plot lines from.
	if (0 > scaled.OL && 0 > scaled.OU) {
		return scaled.OU;
	} else if (0 < scaled.OL && 0 < scaled.OU) {
		return scaled.OL;
	} else {

		// If the axis is in the plot, then we need to create another axis line
		plotData.push_back(makeLineExpression({scaled.AL, scaled.AU, 0, 0}));
	}

	return 0;
}

void addPlotEdges(std::vector<Expression>& plotData, const Bounds& scaled) {

	// Data bounding box edges (top, bottom, left, right)
	plotData.push_back(makeLineExpression({scaled.AL, scaled.AU, scaled.OU, scaled.OU}));
	plotData.push_back(makeLineExpression({scaled.AL, scaled.AU, scaled.OL, scaled.OL}));
	plotData.push_back(makeLineExpression({scaled.AL, scaled.AL, scaled.OU, scaled.OL}));
	plotData.push_back(makeLineExpression({scaled.AU, scaled.AU, scaled.OU, scaled.OL}));
}

void addPlotTickLabels(std::vector<Expression>& plotData, const Bounds& bounds,
	double textScale) {
	Bounds scaled = bounds.scaleForGraphics();

	std::stringstream ss;
	ss << std::setprecision(2);

	// Tick labels (AL, AU, OL, OU)
	ss << bounds.AL;
	plotData.push_back(makeTextExpression(ss.str(), {scaled.AL, scaled.OL - PLOT_C}, textScale));

	ss.str(std::string());
	ss << bounds.AU;
	plotData.push_back(makeTextExpression(ss.str(), {scaled.AU, scaled.OL - PLOT_C}, textScale));

	ss.str(std::string());
	ss << bounds.OL;
	plotData.push_back(makeTextExpression(ss.str(), {scaled.AL - PLOT_D, scaled.OL}, textScale));

	ss.str(std::string());
	ss << bounds.OU;
	plotData.push_back(makeTextExpression(ss.str(), {scaled.AL - PLOT_D, scaled.OU}, textScale));
}

void addScaledDiscreteData(const Expression& data, std::vector<Expression>& plotData,
	const Bounds& bounds, double stemRoot) {
	double absScale = bounds.calcAbsScale();
	double ordScale = bounds.calcOrdScale();

	auto dataBegin = data.tailConstBegin();
	auto dataEnd = data.tailConstEnd();
	for (auto it = dataBegin; it != dataEnd; it++) {
		Point p = getPointValues(*it).scaleForGraphics(absScale, ordScale);

		// Create the scaled point expression and add it to the plot data
		plotData.push_back(makePointExpression(p, PLOT_P / 2));
		plotData.push_back(makeLineExpression({p.x, p.x, stemRoot, p.y}));
	}
}

Expression discretePlot(const std::vector<Expression>& args) {

	// Discrete plots can take one or two arguments (options are, optional)
	bool justData = args.size() == 1;
	bool dataAndOptions = args.size() == 2;

	if (justData || dataAndOptions) {
		const Expression& data = args[0];

		// The data should be a list. Start processing the plot data
		if (data.isHeadListRoot()) {
			Bounds bounds = getBoundsFromList(data);
			Bounds scaledBounds = bounds.scaleForGraphics();
			std::vector<Expression> plotData;

			// We need a value to start stem lines from
			double stemRoot = addPlotAxes(plotData, scaledBounds);

			// for each point in the list of data points, scale and create the line and point objects
			addScaledDiscreteData(data, plotData, bounds, stemRoot);
			addPlotEdges(plotData, scaledBounds);

			// Retreive the options if there were any and apply them (i.e. create the text labels)
			double textScale = 1;
			if (dataAndOptions) {

				// Create the title, abscissa label, ordinate label text objects if they were set and get
				// the text scale
				textScale = handlePlotOptions(plotData, args[1], scaledBounds);
			}

			addPlotTickLabels(plotData, bounds, textScale);

			// if no options exist, just return the plot data
			return Expression(plotData);
		}

		// Both arguements should be a list
		throw SemanticError("Error: arguements to discrete-plot should be lists");
	}

	// This will only get triggered when there is the wrong number of arguments
	throw SemanticError("Error: wrong number of arguments for discrete-plot which "
		"takes one or two arguments");
}

// Helper function to update the ordinate bounds for continuous plots
void updateOrdinateBounds(Bounds& bounds, double o) {
	if (bounds.OL > o) {
		bounds.OL = o;
	} else if (bounds.OU < o) {
		bounds.OU = o;
	}
}

// Helper function to get the point at the evaluated lambda and update the bounds
void stepContinuous(const Expression& lambda, const Environment& env, double toEval, Point& p,
	Bounds& bounds, bool init = false) {
	Expression lambdaResultExp = lambda.evalLambda({Expression(toEval)}, env);

	// if the result is a valid number, then we can make it the next point
	if (lambdaResultExp.isHeadNumber()) {
		p = {toEval, lambdaResultExp.head().asNumber()};

		// Record the maxima for the ordinate axis for scaling (if already initialized)
		if (!init) {
			updateOrdinateBounds(bounds, p.y);
		} else {
			bounds.OL = p.y;
			bounds.OU = p.y;
		}
	} else {
		throw SemanticError("Error: invalid function for continuous plot");
	}
}

double angleAdjacent(const Line& l1, const Line& l2) {
	double angle = 180;

	// Depending on the slope of the line, we need to get different angles to find the angle
	// between the two lines, opposing slopes just add and take inverse, same sign adds
	double m1 = slope(l1);
	double m2 = slope(l2);

	// if one angle is positive and one is negative,
	if ((std::isgreater(m1, 0) && std::isgreater(0, m2)) ||
		(std::isgreater(0, m1) && std::isgreater(m2, 0))) {
		angle = 180 - angleToXAxis(l1) - angleToXAxis(l2);

		// if both are positive or both are negative
	} else if ((std::isgreater(m1, 0) && std::isgreater(m2, 0)) ||
		(std::isgreater(0, m1) && std::isgreater(0, m2))) {

		// if m1 is greater than m2,
		if (std::isgreater(std::abs(m1), std::abs(m2))) {
			angle = 180 + angleToXAxis(l2) - angleToXAxis(l1);

		// if m2 is greater than m1
		} else if (std::isgreater(std::abs(m2), std::abs(m1))) {
			angle = 180 + angleToXAxis(l1) - angleToXAxis(l2);
		}
	}

	return std::abs(angle);
}

void smoothContinuousPlot(const Expression& lambda, const Environment& env,
	std::vector<Line>& lines, Bounds& bounds, std::size_t iteration = 0) {

	// We need to work through the whole plot and split lines that have an angle smaller than 175
	// We do nothing if we already hit 10 iterations
	if (iteration < PLOT_SPLIT_MAX) {
		bool alreadySmooth = true;
		for (std::size_t i = 0; i < lines.size() - 1; i++) {

			// Check the angle between the current line and the next
			Line l1 = lines[i];
			Line l2 = lines[i + 1];
			if (std::isgreater(PLOT_LINE_ANG_MIN, angleAdjacent(l1, l2))) {
				alreadySmooth = false;

				// If the angle is less than the minimum, remove the two current lines, add in the split
				// lines, then advance to the next unevaluated line
				double firstMidx = (l1.x1 + l1.x2) / 2;
				double firstMidy = lambda.evalLambda({Expression(firstMidx)}, env).head().asNumber();
				double secondMidx = (l2.x1 + l2.x2) / 2;
				double secondMidy = lambda.evalLambda({Expression(secondMidx)}, env).head().asNumber();

				// NOTE: It's important to increment the index appropriately to keep the lines in order.
				Line new1 = {l1.x1, firstMidx, l1.y1, firstMidy};
				Line new2 = {firstMidx, l1.x2, firstMidy, l1.y2};
				Line new3 = {l2.x1, secondMidx, l2.y1, secondMidy};
				Line new4 = {secondMidx, l2.x2, secondMidy, l2.y2};

				updateOrdinateBounds(bounds, firstMidy);
				updateOrdinateBounds(bounds, secondMidy);

				// erase the old lines
				lines.erase(lines.begin() + i, lines.begin() + i + 2);

				// insert the new ones in reverse order (insert pushes everything forward)
				lines.insert(lines.begin() + i, new4);
				lines.insert(lines.begin() + i, new3);
				lines.insert(lines.begin() + i, new2);
				lines.insert(lines.begin() + i, new1);
				i += 2;
			}
		}

		if (!alreadySmooth) {
			smoothContinuousPlot(lambda, env, lines, bounds, iteration + 1);
		}
	}
}

void addScaledContinuousData(const Expression& lambda, const Environment& env, Bounds& bounds,
	std::vector<Expression>& plotData) {
	double incValue = (bounds.AU - bounds.AL) / PLOT_M;
	std::vector<Line> lines;

	// Prime the loop
	Point prev, next;
	stepContinuous(lambda, env, bounds.AL, prev, bounds, true);
	for (double i = 1; i < PLOT_M; i++) {

		// create lines from i to i + 1
		stepContinuous(lambda, env, bounds.AL + (incValue * i), next, bounds);

		// add the line to the list
		lines.push_back({prev.x, next.x, prev.y, next.y});
		prev = next;
	}

	// calculate the final value
	stepContinuous(lambda, env, bounds.AU, next, bounds);
	lines.push_back({prev.x, next.x, prev.y, next.y});

	// Smooth the plot
	smoothContinuousPlot(lambda, env, lines, bounds);

	// Iterate through each line, scale it, and add it to the plot
	double absScaleFactor = bounds.calcAbsScale();
	double ordScaleFactor = bounds.calcOrdScale();
	for (auto& line : lines) {
		plotData.push_back(makeLineExpression(line.scaleForGraphics(absScaleFactor, ordScaleFactor)));
	}
}

Expression continuousPlot(const std::vector<Expression>& args, const Environment& env) {

	// Continuous plots can take two or three arguments (options are, optional)
	bool justData = args.size() == 2;
	bool dataAndOptions = args.size() == 3;

	if (justData || dataAndOptions) {
		const Expression& lambda = args[0];

		// Validate the arguments. Start processing the plot data
		if (lambda.isHeadLambdaRoot()) {
			std::vector<Expression> plotData;

			// We can grab the bounds as a point since we expect a list of two values
			Bounds bounds;
			Point abscissaBounds = getPointValues(args[1]);
			bounds.AL = abscissaBounds.x;
			bounds.AU = abscissaBounds.y;

			// Create the plot (addScaledContinuousData will update the bounds)
			addScaledContinuousData(lambda, env, bounds, plotData);

			Bounds scaledBounds = bounds.scaleForGraphics();
			addPlotAxes(plotData, scaledBounds);
			addPlotEdges(plotData, scaledBounds);

			// Retreive the options if there were any and apply them (i.e. create the text labels)
			double textScale = 1;
			if (dataAndOptions) {
				textScale = handlePlotOptions(plotData, args[2], scaledBounds);
			}

			addPlotTickLabels(plotData, bounds, textScale);
			return Expression(plotData);
		}

		throw SemanticError("Error: first argument to continuous-plot should be a lambda function");
	}

	// This will only get triggered when there is the wrong number of arguments
	throw SemanticError("Error: wrong number of arguments for continuous-plot which "
		"takes two or three arguments");
}
