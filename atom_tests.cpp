#include "catch.hpp"

#include "atom.hpp"

TEST_CASE("Test constructors", "[atom]") {

	{
		INFO("Default Constructor");
		Atom a;

		REQUIRE(a.isNone());
		REQUIRE(!a.isNumber());
		REQUIRE(!a.isComplex());
		REQUIRE(!a.isSymbol());
		REQUIRE(!a.isStringLiteral());
	}

	{
		INFO("Number Constructor");
		Atom a(1.0);

		REQUIRE(!a.isNone());
		REQUIRE(a.isNumber());
		REQUIRE(!a.isComplex());
		REQUIRE(!a.isSymbol());
		REQUIRE(!a.isStringLiteral());
	}

	{
		INFO("Complex Constructor");
		Atom a(complex(1, 1));

		REQUIRE(!a.isNone());
		REQUIRE(!a.isNumber());
		REQUIRE(a.isComplex());
		REQUIRE(!a.isSymbol());
		REQUIRE(!a.isStringLiteral());
	}

	{
		INFO("Symbol Constructor");
		Atom a("hi");

		REQUIRE(!a.isNone());
		REQUIRE(!a.isNumber());
		REQUIRE(!a.isComplex());
		REQUIRE(a.isSymbol());
		REQUIRE(!a.isStringLiteral());
	}

	{
		INFO("Symbol Constructor (String Literal)");
		Atom a("\"hi\"");

		REQUIRE(!a.isNone());
		REQUIRE(!a.isNumber());
		REQUIRE(!a.isComplex());
		REQUIRE(!a.isSymbol());
		REQUIRE(a.isStringLiteral());
	}

	{
		INFO("Symbol Constructor (malformed string literal)");

		// This would not be a string literal since there is a quote in between the start and end
		// quotes - this should just create a symbol with the string value passed in
		Atom a("\"h\"i\"");

		REQUIRE(!a.isNone());
		REQUIRE(!a.isNumber());
		REQUIRE(!a.isComplex());
		REQUIRE(a.isSymbol());
		REQUIRE(!a.isStringLiteral());

		REQUIRE(a.asSymbol() == "\"h\"i\"");
	}

	{
		INFO("Token Constructor");
		Token t("hi");
		Atom a(t);

		REQUIRE(!a.isNone());
		REQUIRE(!a.isNumber());
		REQUIRE(!a.isComplex());
		REQUIRE(a.isSymbol());
		REQUIRE(!a.isStringLiteral());
	}

	{
		INFO("Copy Constructor");
		Atom a("hi");
		Atom b(1.0);
		Atom c(complex(1, 1));
		Atom d("\"hi\"");

		Atom z(a);
		REQUIRE(!z.isNone());
		REQUIRE(!z.isNumber());
		REQUIRE(!z.isComplex());
		REQUIRE(z.isSymbol());
		REQUIRE(!z.isStringLiteral());

		Atom y(b);
		REQUIRE(!y.isNone());
		REQUIRE(y.isNumber());
		REQUIRE(!y.isComplex());
		REQUIRE(!y.isSymbol());
		REQUIRE(!y.isStringLiteral());

		Atom x(c);
		REQUIRE(!x.isNone());
		REQUIRE(!x.isNumber());
		REQUIRE(x.isComplex());
		REQUIRE(!x.isSymbol());
		REQUIRE(!x.isStringLiteral());

		Atom w(d);
		REQUIRE(!w.isNone());
		REQUIRE(!w.isNumber());
		REQUIRE(!w.isComplex());
		REQUIRE(!w.isSymbol());
		REQUIRE(w.isStringLiteral());
	}
}

TEST_CASE("Test assignment", "[atom]") {

	{
		INFO("default to default");
		Atom a;
		Atom b;
		b = a;
		REQUIRE(b.isNone());
		REQUIRE(!b.isNumber());
		REQUIRE(!b.isComplex());
		REQUIRE(!b.isSymbol());
		REQUIRE(!b.isStringLiteral());
	}

	{
		INFO("default to number");
		Atom a;
		Atom b(1.0);
		b = a;
		REQUIRE(b.isNone());
		REQUIRE(!b.isNumber());
		REQUIRE(!b.isComplex());
		REQUIRE(!b.isSymbol());
		REQUIRE(!b.isStringLiteral());
	}

	{
		INFO("default to complex");
		Atom a;
		Atom b(complex(1, 1));
		b = a;
		REQUIRE(b.isNone());
		REQUIRE(!b.isNumber());
		REQUIRE(!b.isComplex());
		REQUIRE(!b.isSymbol());
		REQUIRE(!b.isStringLiteral());
	}

	{
		INFO("default to symbol");
		Atom a;
		Atom b("hi");
		b = a;
		REQUIRE(b.isNone());
		REQUIRE(!b.isNumber());
		REQUIRE(!b.isComplex());
		REQUIRE(!b.isSymbol());
		REQUIRE(!b.isStringLiteral());
	}

	{
		INFO("number to default");
		Atom a(1.0);
		Atom b;
		b = a;
		REQUIRE(b.isNumber());
		REQUIRE(b.asNumber() == 1.0);
	}

	{
		INFO("number to number");
		Atom a(1.0);
		Atom b(2.0);
		b = a;
		REQUIRE(b.isNumber());
		REQUIRE(b.asNumber() == 1.0);
	}

	{
		INFO("number to complex");
		Atom a(1.0);
		Atom b(complex(1, 1));
		b = a;
		REQUIRE(b.isNumber());
		REQUIRE(b.asNumber() == 1.0);
	}

	{
		INFO("number to symbol");
		Atom a(1.0);
		Atom b("hi");
		b = a;
		REQUIRE(b.isNumber());
		REQUIRE(b.asNumber() == 1.0);
	}

	{
		INFO("symbol to default");
		Atom a("hi");
		Atom b;
		b = a;
		REQUIRE(b.isSymbol());
		REQUIRE(b.asSymbol() == "hi");
	}

	{
		INFO("symbol to number");
		Atom a("hi");
		Atom b(1.0);
		b = a;
		REQUIRE(b.isSymbol());
		REQUIRE(b.asSymbol() == "hi");
	}

	{
		INFO("symbol to complex");
		Atom a("hi");
		Atom b(complex(1, 1));
		b = a;
		REQUIRE(b.isSymbol());
		REQUIRE(b.asSymbol() == "hi");
	}

	{
		INFO("symbol to symbol");
		Atom a("hi");
		Atom b("bye");
		b = a;
		REQUIRE(b.isSymbol());
		REQUIRE(b.asSymbol() == "hi");
	}

	{
		INFO("complex to default");
		Atom a(complex(1, 1));
		Atom b;
		b = a;
		REQUIRE(b.isComplex());
		REQUIRE(b.asComplex() == complex(1, 1));
	}

	{
		INFO("complex to number");
		Atom a(complex(1, 1));
		Atom b(1.0);
		b = a;
		REQUIRE(b.isComplex());
		REQUIRE(b.asComplex() == complex(1, 1));
	}

	{
		INFO("complex to complex");
		Atom a(complex(1, 1));
		Atom b(complex(2, 2));
		b = a;
		REQUIRE(b.isComplex());
		REQUIRE(b.asComplex() == complex(1, 1));
	}

	{
		INFO("complex to symbol");
		Atom a(complex(1, 1));
		Atom b("hi");
		b = a;
		REQUIRE(b.isComplex());
		REQUIRE(b.asComplex() == complex(1, 1));
	}

	{
		INFO("string literal to default");
		Atom a("\"hi\"");
		Atom b;
		b = a;
		REQUIRE(b.isStringLiteral());
		REQUIRE(b.asSymbol() == "\"hi\"");
	}

	{
		INFO("string literal to number");
		Atom a("\"hi\"");
		Atom b(1.0);
		b = a;
		REQUIRE(b.isStringLiteral());
		REQUIRE(b.asSymbol() == "\"hi\"");
	}

	{
		INFO("string literal to complex");
		Atom a("\"hi\"");
		Atom b(complex(2, 2));
		b = a;
		REQUIRE(b.isStringLiteral());
		REQUIRE(b.asSymbol() == "\"hi\"");
	}

	{
		INFO("string literal to symbol");
		Atom a("\"hi\"");
		Atom b("hi");
		b = a;
		REQUIRE(b.isStringLiteral());
		REQUIRE(b.asSymbol() == "\"hi\"");
	}

	{
		INFO("string literal to string literal");
		Atom a("\"hi\"");
		Atom b("\"bye\"");
		b = a;
		REQUIRE(b.isStringLiteral());
		REQUIRE(b.asSymbol() == "\"hi\"");
	}
}

TEST_CASE("test comparison", "[atom]") {

	{
		INFO("compare default to default");
		Atom a;
		Atom b;
		REQUIRE(a == b);
	}

	{
		INFO("compare default to number");
		Atom a;
		Atom b(1.0);
		REQUIRE(a != b);
	}

	{
		INFO("compare default to complex");
		Atom a;
		Atom b(complex(1, 1));
		REQUIRE(a != b);
	}

	{
		INFO("compare default to symbol");
		Atom a;
		Atom b("hi");
		REQUIRE(a != b);
	}

	{
		INFO("compare default to string literal");
		Atom a;
		Atom b("\"hi\"");
		REQUIRE(a != b);
	}

	{
		INFO("compare number to default");
		Atom a(1.0);
		Atom b;
		REQUIRE(a != b);
	}

	{
		INFO("compare number to number");
		Atom a(1.0);
		Atom b(1.0);
		Atom c(2.0);
		REQUIRE(a == b);
		REQUIRE(a != c);
	}

	{
		INFO("compare number to complex");
		Atom a(1.0);
		Atom b(complex(1, 1));
		REQUIRE(a != b);
	}

	{
		INFO("compare number to symbol");
		Atom a(1.0);
		Atom b("hi");
		REQUIRE(a != b);
	}

	{
		INFO("compare number to string literal");
		Atom a(1.0);
		Atom b("\"hi\"");
		REQUIRE(a != b);
	}

	{
		INFO("compare symbol to default");
		Atom a("hi");
		Atom b;
		REQUIRE(a != b);
	}

	{
		INFO("compare symbol to number");
		Atom a("hi");
		Atom b(1.0);
		REQUIRE(a != b);
	}

	{
		INFO("compare symbol to complex");
		Atom a("hi");
		Atom b(complex(1, 1));
		REQUIRE(a != b);
	}

	{
		INFO("compare symbol to symbol");
		Atom a("hi");
		Atom b("hi");
		Atom c("bye");
		REQUIRE(a == b);
		REQUIRE(a != c);
	}

	{
		INFO("compare symbol to string literal");
		Atom a("hi");
		Atom b("\"hi\"");
		REQUIRE(a != b);
	}

	{
		INFO("compare complex to default");
		Atom a(complex(1, 1));
		Atom b;
		REQUIRE(a != b);
	}

	{
		INFO("compare complex to number");
		Atom a(complex(1, 1));
		Atom b(1.0);
		REQUIRE(a != b);
	}

	{
		INFO("compare complex to complex");
		Atom a(complex(1, 1));
		Atom b(complex(1, 1));
		Atom c(complex(1, 2));
		REQUIRE(a == b);
		REQUIRE(a != c);
	}

	{
		INFO("compare complex to string literal");
		Atom a(complex(1, 1));
		Atom b("\"hi\"");
		REQUIRE(a != b);
	}

	{
		INFO("compare string literal to default");
		Atom a("\"hi\"");
		Atom b;
		REQUIRE(a != b);
	}

	{
		INFO("compare string literal to number");
		Atom a("\"hi\"");
		Atom b(1.0);
		REQUIRE(a != b);
	}

	{
		INFO("compare string literal to complex");
		Atom a("\"hi\"");
		Atom b(complex(1, 1));
		REQUIRE(a != b);
	}

	{
		INFO("compare string literal to symbol");
		Atom a("\"hi\"");
		Atom b("hi");
		REQUIRE(a != b);
	}

	{
		INFO("compare string literal to string literal");
		Atom a("\"hi\"");
		Atom b("\"hi\"");
		REQUIRE(a == b);
	}
}

TEST_CASE("Retrieving Atoms as a certain type", "[atom]") {

	{
		INFO("number as number");
		Atom a(1.7);
		REQUIRE(a.asNumber() == 1.7);
	}

	{
		INFO("number as complex");
		Atom a(1.7);
		REQUIRE(a.asComplex() == complex(1.7, 0));
	}

	{
		INFO("number as symbol");
		Atom a(1.7);
		REQUIRE(a.asSymbol() == "");
	}

	{
		INFO("complex as number");
		Atom a(complex(0, 1));
		REQUIRE(a.asNumber() == 0);
	}

	{
		INFO("complex as complex");
		Atom a(complex(0, 1));
		REQUIRE(a.asComplex() == complex(0, 1));
	}

	{
		INFO("complex as symbol");
		Atom a(complex(0, 1));
		REQUIRE(a.asSymbol() == "");
	}

	{
		INFO("symbol as number");
		Atom a("A");
		REQUIRE(a.asNumber() == 0);
	}

	{
		INFO("symbol as complex");
		Atom a("A");
		REQUIRE(a.asComplex() == complex(0, 0));
	}

	{
		INFO("symbol as symbol");
		Atom a("A");
		REQUIRE(a.asSymbol() == "A");
	}

	{
		INFO("string literal as number");
		Atom a("\"hi\"");
		REQUIRE(a.asNumber() == 0);
	}

	{
		INFO("string literal as complex");
		Atom a("\"hi\"");
		REQUIRE(a.asComplex() == complex(0, 0));
	}

	{
		INFO("string literal as symbol");
		Atom a("\"hi\"");
		REQUIRE(a.asSymbol() == "\"hi\"");
	}

	{
		INFO("string literal as symbol");
		Atom a("\"hi\"");
		REQUIRE(a.asSymbol() == "\"hi\"");
	}
}
