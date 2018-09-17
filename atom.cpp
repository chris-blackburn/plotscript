#include "atom.hpp"

#include <sstream>
#include <cctype>
#include <cmath>
#include <limits>

Atom::Atom(): m_type(NoneKind), atomList(nullptr) {}

Atom::Atom(double value): Atom() {
	setNumber(value);
}

Atom::Atom(complex value): Atom() {
	setComplex(value);
}

Atom::Atom(const std::list<Atom>& value): Atom() {
	setList(value);
}

Atom::Atom(const Token& token): Atom() {

	// is token a number?
	double temp;
	std::istringstream iss(token.asString());
	if (iss >> temp) {

		// check for trailing characters if >> succeeds, else assume symbol
		if (iss.rdbuf()->in_avail() == 0) {
			setNumber(temp);
		}
	} else {

		// make sure does not start with number
		if (!std::isdigit(token.asString()[0])) {
			setSymbol(token.asString());
		}
	}
}

Atom::Atom(const std::string& value): Atom() {
	setSymbol(value);
}

Atom::Atom(const Atom& x): Atom() {
	if (x.isNumber()) {
		setNumber(x.numberValue);
	} else if (x.isComplex()) {
		setComplex(x.complexValue);
	} else if (x.isSymbol()) {
		setSymbol(x.stringValue);
	} else if (x.isList()) {
		setList(*x.atomList);
	}
}

Atom& Atom::operator=(const Atom& x) {
	if (this != &x) {
		if (x.m_type == NoneKind) {
			m_type = NoneKind;
		} else if (x.m_type == NumberKind) {
			setNumber(x.numberValue);
		} else if (x.m_type == ComplexKind) {
			setComplex(x.complexValue);
		} else if (x.m_type == SymbolKind) {
			setSymbol(x.stringValue);
		} else if (x.m_type == ListKind) {
			setList(*x.atomList);
		}
	}

	return *this;
}

Atom::~Atom() {

	// we need to ensure certain destructors are called like string
	switch(m_type) {
	case SymbolKind:
		stringValue.~basic_string();
		break;
	case ListKind:
		delete atomList;
		break;
	default:
		break;
	}
}

bool Atom::isNone() const noexcept {
	return m_type == NoneKind;
}

bool Atom::isNumber() const noexcept {
	return m_type == NumberKind;
}

bool Atom::isComplex() const noexcept {
	return m_type == ComplexKind;
}

bool Atom::isSymbol() const noexcept {
	return m_type == SymbolKind;
}

bool Atom::isList() const noexcept {
	return m_type == ListKind;
}

double Atom::truncateToZero(double value) {

	// if the value is smaller than or equal to epsilon, just make it zero
	if (fabs(value) <= std::numeric_limits<double>::epsilon()) {
		return 0;
	}

	return value;
}

void Atom::setNumber(double value) {
	m_type = NumberKind;

	numberValue = truncateToZero(value);
}

void Atom::setComplex(complex value) {
	m_type = ComplexKind;

	complexValue = complex(truncateToZero(value.real()), truncateToZero(value.imag()));
}

void Atom::setSymbol(const std::string& value) {

	// we need to ensure the destructor of the symbol string is called
	if (m_type == SymbolKind) {
		stringValue.~basic_string();
	}

	m_type = SymbolKind;

	// copy construct in place
	new (&stringValue) std::string(value);
}

void Atom::setList(const std::list<Atom>& value) {

	// Delete the old list
	if (m_type == ListKind) {
		delete atomList;
	}

	m_type = ListKind;

	atomList = new std::list<Atom>(value);
}

double Atom::asNumber() const noexcept {
	return (m_type == NumberKind) ? numberValue : 0.0;
}

complex Atom::asComplex() const noexcept {
	switch(m_type) {
	case ComplexKind:
		return complexValue;
	case NumberKind:
		return complex(numberValue, 0);
	default:
		return complex(0, 0);
	}
}

std::string Atom::asSymbol() const noexcept {
	std::string result;

	if (m_type == SymbolKind) {
		result = stringValue;
	}

	return result;
}

std::list<Atom> Atom::asList() const noexcept {
	std::list<Atom> result;

	if (m_type == ListKind) {
		result = *atomList;
	}

	return result;
}

bool Atom::operator==(const Atom& right) const noexcept {
	if (m_type != right.m_type) {
		return false;
	}

	switch(m_type) {
	case NoneKind:

		// Avoid the default case for NoneKind - if both are NoneKind, then it will return
		// true once outside this switch statement
		break;
	case NumberKind:
		{
			double dleft = numberValue;
			double dright = right.numberValue;
			double diff = fabs(dleft - dright);

			if(std::isnan(diff) || (diff > std::numeric_limits<double>::epsilon())) {
				return false;
			}
		}

		break;
	case ComplexKind:
		return complexValue == right.complexValue;
	case SymbolKind:
		return stringValue == right.stringValue;
	case ListKind:
		return *atomList == *right.atomList;
	default:
		return false;
	}

	return true;
}

bool operator!=(const Atom& left, const Atom& right) noexcept {
	return !(left == right);
}

std::ostream& operator<<(std::ostream& out, const Atom& a) {
	if (a.isNumber()) {
		out << a.asNumber();
	} else if (a.isComplex()) {
		out << a.asComplex().real() << "," << a.asComplex().imag();
	} else if (a.isSymbol()) {
		out << a.asSymbol();
	} else if (a.isList()) {

		// Handle printing out list of atoms recursively
		for (auto& atom : a.asList()) {
			out << atom;
		}
	}

	return out;
}
