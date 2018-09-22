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
		REQUIRE(!a.isListRoot());
	}

	{
		INFO("Default Constructor (set as root)");
		Atom a;
		a.setListRoot();

		REQUIRE(!a.isNone());
		REQUIRE(!a.isNumber());
		REQUIRE(!a.isComplex());
		REQUIRE(!a.isSymbol());
		REQUIRE(a.isListRoot());
	}

	{
		INFO("Number Constructor");
		Atom a(1.0);

		REQUIRE(!a.isNone());
		REQUIRE(a.isNumber());
		REQUIRE(!a.isComplex());
		REQUIRE(!a.isSymbol());
		REQUIRE(!a.isListRoot());
	}

	{
		INFO("Complex Constructor");
		Atom a(complex(1, 1));

		REQUIRE(!a.isNone());
		REQUIRE(!a.isNumber());
		REQUIRE(a.isComplex());
		REQUIRE(!a.isSymbol());
		REQUIRE(!a.isListRoot());
	}

	{
		INFO("Symbol Constructor");
		Atom a("hi");

		REQUIRE(!a.isNone());
		REQUIRE(!a.isNumber());
		REQUIRE(!a.isComplex());
		REQUIRE(a.isSymbol());
		REQUIRE(!a.isListRoot());
	}

	{
		INFO("Token Constructor");
		Token t("hi");
		Atom a(t);

		REQUIRE(!a.isNone());
		REQUIRE(!a.isNumber());
		REQUIRE(!a.isComplex());
		REQUIRE(a.isSymbol());
		REQUIRE(!a.isListRoot());
	}

	{
		INFO("Copy Constructor");
		Atom a("hi");
		Atom b(1.0);
		Atom c(complex(1, 1));
		Atom d;
		d.setListRoot();

		Atom e(a);
		REQUIRE(!e.isNone());
		REQUIRE(!e.isNumber());
		REQUIRE(!e.isComplex());
		REQUIRE(e.isSymbol());
		REQUIRE(!e.isListRoot());

		Atom f(b);
		REQUIRE(!f.isNone());
		REQUIRE(f.isNumber());
		REQUIRE(!f.isComplex());
		REQUIRE(!f.isSymbol());
		REQUIRE(!f.isListRoot());

		Atom g(c);
		REQUIRE(!g.isNone());
		REQUIRE(!g.isNumber());
		REQUIRE(g.isComplex());
		REQUIRE(!g.isSymbol());
		REQUIRE(!g.isListRoot());

		Atom h(d);
		REQUIRE(!h.isNone());
		REQUIRE(!h.isNumber());
		REQUIRE(!h.isComplex());
		REQUIRE(!h.isSymbol());
		REQUIRE(h.isListRoot());
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
		REQUIRE(!b.isListRoot());
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
		REQUIRE(!b.isListRoot());
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
		REQUIRE(!b.isListRoot());
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
		REQUIRE(!b.isListRoot());
	}

	{
		INFO("default to root");
		Atom a;
		Atom b;
		b.setListRoot();
		b = a;
		REQUIRE(b.isNone());
		REQUIRE(!b.isNumber());
		REQUIRE(!b.isComplex());
		REQUIRE(!b.isSymbol());
		REQUIRE(!b.isListRoot());
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
		INFO("number to root");
		Atom a(1.0);
		Atom b;
		b.setListRoot();
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
		INFO("root to default");
		Atom a;
		a.setListRoot();
		Atom b;
		b = a;
		REQUIRE(b.isListRoot());
	}

	{
		INFO("root to number");
		Atom a;
		a.setListRoot();
		Atom b(1.0);
		b = a;
		REQUIRE(b.isListRoot());
	}

	{
		INFO("root to complex")
		Atom a;
		a.setListRoot();
		Atom b(complex(1, 1));
		b = a;
		REQUIRE(b.isListRoot());
	}

	{
		INFO("root to symbol")
		Atom a;
		a.setListRoot();
		Atom b("hi");
		b = a;
		REQUIRE(b.isListRoot());
	}

	{
		INFO("root to root")
		Atom a;
		a.setListRoot();
		Atom b;
		b.setListRoot();
		b = a;
		REQUIRE(b.isListRoot());
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
		INFO("compare default to root");
		Atom a;
		Atom b;
		b.setListRoot();
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
		INFO("compare number to root");
		Atom a(1.0);
		Atom b;
		b.setListRoot();
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
		INFO("compare symbol to root");
		Atom a("hi");
		Atom b;
		b.setListRoot();
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
		INFO("compare complex to root");
		Atom a("hi");
		Atom b;
		b.setListRoot();
		REQUIRE(a != b);
	}

	{
		INFO("Compare root to default") {
			Atom a;
			a.setListRoot();
			Atom b;
			REQUIRE(a != b);
		}
	}

	{
		INFO("Compare root to number") {
			Atom a;
			a.setListRoot();
			Atom b(1.0);
			REQUIRE(a != b);
		}
	}

	{
		INFO("Compare root to complex") {
			Atom a;
			a.setListRoot();
			Atom b(complex(1, 1));
			REQUIRE(a != b);
		}
	}

	{
		INFO("Compare root to symbol") {
			Atom a;
			a.setListRoot();
			Atom b("hi");
			REQUIRE(a != b);
		}
	}

	{
		INFO("Compare root to root") {
			Atom a;
			a.setListRoot();
			Atom b;
			b.setListRoot();
			REQUIRE(a == b);
		}
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
		INFO("root as number");
		Atom a;
		a.setListRoot();
		REQUIRE(a.asNumber() == 0);
	}

	{
		INFO("root as complex");
		Atom a;
		a.setListRoot();
		REQUIRE(a.asComplex() == complex(0, 0));
	}

	{
		INFO("root as symbol");
		Atom a;
		a.setListRoot();
		REQUIRE(a.asSymbol() == "");
	}
}
