/*! \file atom.hpp
Defines the Atom type and associated functions.
 */
#ifndef ATOM_HPP
#define ATOM_HPP

#include "token.hpp"

// abstracts our complex number type
#include <complex>
using complex = std::complex<double>;

/*! \class Atom
\brief A variant type that may be a Number or Symbol or the default type None.

This class provides value semantics.
*/
class Atom {
public:

	/// Construct a default Atom of type None
	Atom();

	/// Construct an Atom of type Number with value
	Atom(double value);

	/// Construct an Atom of type Complex with value
	Atom(complex value);

	/// Construct an Atom of type Symbol named value
	Atom(const std::string& value);

	/// Construct an Atom directly from a Token
	Atom(const Token& token);

	/// Copy-construct an Atom
	Atom(const Atom& x);

	/// Assign an Atom
	Atom& operator=(const Atom& x);

	/// Atom destructor
	~Atom();

	/// predicate to determine if an Atom is of type None
	bool isNone() const noexcept;

	/// predicate to determine if an Atom is of type Number
	bool isNumber() const	noexcept;

	/// predicate to determine if an Atom is of type Complex
	bool isComplex() const noexcept;

	/// predicate to determine if an Atom is of type Symbol
	bool isSymbol() const noexcept;

	/// predicate to determine if an Atom is of type StringLiteral
	bool isStringLiteral() const noexcept;

	/// value of Atom as a number, return 0 if not a Number
	double asNumber() const noexcept;

	/// value of Atom as a complex, return (0, 0) if not a Complex Number
	/// If the Atom is a number, then it returns the complex verson of that number (n, 0)
	complex asComplex() const noexcept;

	/// value of Atom as a string, returns empty-string if not a Symbol
	std::string asSymbol() const noexcept;

	/// equality comparison based on type and value
	bool operator==(const Atom& right) const noexcept;

private:

	// internal enum of known types
	// ListRootKind atoms have no value - they are used to mark the head of an expression AST
	enum Type {NoneKind, NumberKind, ComplexKind, SymbolKind, StringLiteralKind};

	// track the type
	Type m_type;

	// values for the known types. Note the use of a union requires care
	// when setting non POD values (see setSymbol)
	union {
		double numberValue;
		std::string stringValue;
		complex complexValue;
	};

	// Helper function for copy construct and copy assignment
	void copy(const Atom& x);

	// Helper function to make numbers smaller than or equal to epsilon equal to zero
	double truncateToZero(double value);

	// helper to set type and value of Number
	void setNumber(double value);

	// helper to set the type and value of a complex number
	void setComplex(complex value);

	// helper to set type and value of Symbol. If the string is surrounded by quotes, it
	// will set it as a string literal.
	void setSymbol(const std::string& value);

	// helper to set type and value of String Literal.
	void setStringLiteral(const std::string& value);
};

/// inequality comparison for Atom
bool operator!=(const Atom& left, const Atom& right) noexcept;

/// output stream rendering
std::ostream & operator<<(std::ostream& out, const Atom& a);

#endif
