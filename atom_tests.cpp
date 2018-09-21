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
		REQUIRE(!a.isRoot());
	}

	{
		INFO("Default Constructor (set as root)");
		Atom a;
		REQUIRE(a.setRoot());

		REQUIRE(!a.isNone());
		REQUIRE(!a.isNumber());
		REQUIRE(!a.isComplex());
		REQUIRE(!a.isSymbol());
		REQUIRE(a.isRoot());

		// setRoot on RootKind still returns true
		REQUIRE(a.setRoot());
	}

	{
		INFO("Number Constructor");
		Atom a(1.0);

		REQUIRE(!a.isNone());
		REQUIRE(a.isNumber());
		REQUIRE(!a.isComplex());
		REQUIRE(!a.isSymbol());
		REQUIRE(!a.isRoot());

		REQUIRE(!a.setRoot());
	}

	{
		INFO("Complex Constructor");
		Atom a(complex(1, 1));

		REQUIRE(!a.isNone());
		REQUIRE(!a.isNumber());
		REQUIRE(a.isComplex());
		REQUIRE(!a.isSymbol());
		REQUIRE(!a.isRoot());

		REQUIRE(!a.setRoot());
	}

	{
		INFO("Symbol Constructor");
		Atom a("hi");

		REQUIRE(!a.isNone());
		REQUIRE(!a.isNumber());
		REQUIRE(!a.isComplex());
		REQUIRE(a.isSymbol());
		REQUIRE(!a.isRoot());

		REQUIRE(!a.setRoot());
	}

	{
		INFO("Token Constructor");
		Token t("hi");
		Atom a(t);

		REQUIRE(!a.isNone());
		REQUIRE(!a.isNumber());
		REQUIRE(!a.isComplex());
		REQUIRE(a.isSymbol());
		REQUIRE(!a.isRoot());

		REQUIRE(!a.setRoot());
	}

	{
		INFO("Copy Constructor");
		Atom a("hi");
		Atom b(1.0);
		Atom c(complex(1, 1));

		Atom d = a;
		REQUIRE(!d.isNone());
		REQUIRE(!d.isNumber());
		REQUIRE(!d.isComplex());
		REQUIRE(d.isSymbol());

		Atom e = b;
		REQUIRE(!e.isNone());
		REQUIRE(e.isNumber());
		REQUIRE(!e.isComplex());
		REQUIRE(!e.isSymbol());

		Atom f = c;
		REQUIRE(!f.isNone());
		REQUIRE(!f.isNumber());
		REQUIRE(f.isComplex());
		REQUIRE(!f.isSymbol());
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
		Atom a("hi");
		Atom b(1.0);
		b = a;
		REQUIRE(b.isSymbol());
		REQUIRE(b.asSymbol() == "hi");
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
		Atom a(complex(1, 2));
		Atom b(complex(1, 1));
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
}
