/*! \file expression.hpp
Defines the Expression type and assiciated functions.
 */
#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#include "token.hpp"
#include "atom.hpp"

// forward declare Environment
class Environment;

/*! \class Expression
\brief An expression is a tree of Atoms.

An expression is an atom called the head followed by a (possibly empty)
list of expressions called the tail.
 */
class Expression {
public:
	typedef std::vector<Expression>::const_iterator ConstIteratorType;

	/// Default construct and Expression, whose type in NoneType
	Expression();

	/// Constructor for an list of expressions.
	Expression(const std::vector<Expression>& a);

	/*! Construct an Expression with given Atom as head an empty tail
		\param atom the atom to make the head
	*/
	Expression(const Atom& a);

	/// deep-copy construct an expression (recursive)
	Expression(const Expression& a);

	/// deep-copy assign an expression	(recursive)
	Expression& operator=(const Expression& a);

	/// return a reference to the head Atom
	Atom& head();

	/// return a const-reference to the head Atom
	const Atom& head() const;

	/// append Atom to tail of the expression
	void append(const Atom& a);

	/// return a pointer to the last expression in the tail, or nullptr
	Expression* tail();

	/// return a const-iterator to the beginning of tail
	ConstIteratorType tailConstBegin() const noexcept;

	/// return a const-iterator to the tail end
	ConstIteratorType tailConstEnd() const noexcept;

	/// convienience member to determine if head atom is a number
	bool isHeadNumber() const noexcept;

	/// convienience member to determine if head atom is complex
	bool isHeadComplex() const noexcept;

	/// convienience member to determine if head atom is a symbol
	bool isHeadSymbol() const noexcept;

	/// convienience member to determine if head atom is a string literal
	bool isHeadStringLiteral() const noexcept;

	/// convienience member to determine if head atom is the root of a list
	bool isHeadListRoot() const noexcept;

	/// convienience member to determine if head atom is the root of a lambda expression
	bool isHeadLambdaRoot() const noexcept;

	/// Creates a new property for this expression with key and value
	void setProperty(const std::string& key, const Expression& value);

	/// return the value of a certain property of the expression. If no such property exists,
	/// an empty expression is returned
	Expression getProperty(const std::string& property) const;

	/// Evaluate expression using a post-order traversal (recursive)
	Expression eval(Environment& env);

	/// Evaluate a lambda function with a certain input expression
	Expression evalLambda(const std::vector<Expression>& input, const Environment& env) const;

	/// equality comparison for two expressions (recursive)
	bool operator==(const Expression& exp) const noexcept;

private:

	// the head of the expression
	Atom m_head;

	// the tail list is expressed as a vector for access efficiency
	// and cache coherence, at the cost of wasted memory.
	std::vector<Expression> m_tail;

	// Map of properties linked to this expression. I used a pointer here because not every
	// expression will make use of this property list
	typedef std::unordered_map<std::string, Expression> PropertyMap;
	std::unique_ptr<PropertyMap> m_props;

	// convenience typedef
	typedef std::vector<Expression>::iterator IteratorType;

	// Macros for the heads of special types of expressions.
	#define ListRoot Atom("list")
	#define LambdaRoot Atom("lambda")

	// internal helper method to determin if an Atom is a special form (list, begin, define, etc.)
	bool isSpecialForm(const Atom& head) const;

	// internal helper methods
	Expression handle_lookup(const Atom& head, const Environment& env);
	Expression handle_define(Environment& env);
	Expression handle_begin(Environment& env);
	Expression handle_list(Environment& env);
	Expression handle_lambda(Environment& env);
	Expression handle_apply(Environment& env);
	Expression handle_map(Environment& env);
	Expression handle_setProperty(Environment& env);
	Expression handle_getProperty(Environment& env);
};

/// Render expression to output stream
std::ostream & operator<<(std::ostream& out, const Expression& exp);

/// inequality comparison for two expressions (recursive)
bool operator!=(const Expression& left, const Expression& right) noexcept;

#endif
