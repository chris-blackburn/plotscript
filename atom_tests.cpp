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
		REQUIRE(!a.isList());
	}

	{
		INFO("Number Constructor");
		Atom a(1.0);

		REQUIRE(!a.isNone());
		REQUIRE(a.isNumber());
		REQUIRE(!a.isComplex());
		REQUIRE(!a.isSymbol());
		REQUIRE(!a.isList());
	}

	{
		INFO("Complex Constructor");
		Atom a(complex(1, 1));

		REQUIRE(!a.isNone());
		REQUIRE(!a.isNumber());
		REQUIRE(a.isComplex());
		REQUIRE(!a.isSymbol());
		REQUIRE(!a.isList());
	}

	{
		INFO("Symbol Constructor");
		Atom a("hi");

		REQUIRE(!a.isNone());
		REQUIRE(!a.isNumber());
		REQUIRE(!a.isComplex());
		REQUIRE(a.isSymbol());
		REQUIRE(!a.isList());
	}

	{
		INFO("List Constructor");
		Atom a({Atom(1), Atom(7.1), Atom("a"), Atom(complex(0, 1))});

		REQUIRE(!a.isNone());
		REQUIRE(!a.isNumber());
		REQUIRE(!a.isComplex());
		REQUIRE(!a.isSymbol());
		REQUIRE(a.isList());
	}

	// TODO: Write tests for inputting complex number tokens
	{
		INFO("Token Constructor");
		Token t("hi");
		Atom a(t);

		REQUIRE(!a.isNone());
		REQUIRE(!a.isNumber());
		REQUIRE(!a.isComplex());
		REQUIRE(a.isSymbol());
	}

	{
		INFO("Copy Constructor");
		Atom a("hi");
		Atom b(1.0);
		Atom c(complex(1, 1));
		Atom d({a, b, c});

		Atom e = a;
		REQUIRE(!e.isNone());
		REQUIRE(!e.isNumber());
		REQUIRE(!e.isComplex());
		REQUIRE(e.isSymbol());
		REQUIRE(!e.isList());

		Atom f = b;
		REQUIRE(!f.isNone());
		REQUIRE(f.isNumber());
		REQUIRE(!f.isComplex());
		REQUIRE(!f.isSymbol());
		REQUIRE(!f.isList());

		Atom g = c;
		REQUIRE(!g.isNone());
		REQUIRE(!g.isNumber());
		REQUIRE(g.isComplex());
		REQUIRE(!g.isSymbol());
		REQUIRE(!g.isList());

		Atom h = d;
		REQUIRE(!h.isNone());
		REQUIRE(!h.isNumber());
		REQUIRE(!h.isComplex());
		REQUIRE(!h.isSymbol());
		REQUIRE(h.isList());
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
		REQUIRE(!a.isList());
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
		REQUIRE(!a.isList());
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
		REQUIRE(!a.isList());
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
		REQUIRE(!a.isList());
	}

	{
		INFO("default to list");
		Atom a;
		Atom b(std::list<Atom>{a});
		b = a;
		REQUIRE(b.isNone());
		REQUIRE(!b.isNumber());
		REQUIRE(!b.isComplex());
		REQUIRE(!b.isSymbol());
		REQUIRE(!a.isList());
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
		INFO("number to list");
		Atom a(1.0);
		Atom b(std::list<Atom>{a});
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
		INFO("symbol to list");
		Atom a("hi");
		Atom b(std::list<Atom>{a});
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
		INFO("complex to list");
		Atom a(complex(1, 1));
		Atom b(std::list<Atom>{a});
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
		INFO("compare default to list");
		Atom a;
		Atom b(std::list<Atom>{});
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
		INFO("compare number to list");
		Atom a(1.0);
		Atom b(std::list<Atom>{a});
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
		INFO("symbol number to list");
		Atom a("hi");
		Atom b(std::list<Atom>{a});
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
		INFO("compare complex to symbol");
		Atom a(complex(1, 2));
		Atom b("hi");
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
		INFO("compare complex to list");
		Atom a(complex(1, 1));
		Atom b(std::list<Atom>{Atom(1.0)});
		REQUIRE(a != b);
	}

	{
		INFO("compare list to default");
		Atom a(std::list<Atom>{});
		Atom b;
		REQUIRE(a != b);
	}

	{
		INFO("compare list to number");
		Atom a(std::list<Atom>{});
		Atom b(1.0);
		REQUIRE(a != b);
	}

	{
		INFO("compare list to complex");
		Atom a(std::list<Atom>{});
		Atom b(complex(1, 1));
		REQUIRE(a != b);
	}

	{
		INFO("compare list to symbol");
		Atom a(std::list<Atom>{});
		Atom b("hi");
		REQUIRE(a != b);
	}

	{
		INFO("compare list to list");
		Atom a(std::list<Atom>{Atom(1.0), Atom("hi"), Atom(complex(1, 1))});
		Atom b(std::list<Atom>{Atom(1.0), Atom("hi"), Atom(complex(1, 1))});
		Atom c(std::list<Atom>{Atom(1.0), Atom("bye"), Atom(complex(1, 1))});

		// embedded lists
		Atom d(std::list<Atom>{Atom(1.0), std::list<Atom>{Atom("bye"), Atom(complex(1, 1))}});
		Atom e(std::list<Atom>{Atom(1.0), std::list<Atom>{Atom("bye"), Atom(complex(1, 1))}});
		REQUIRE(a == b);
		REQUIRE(a != c);
		REQUIRE(d == e);
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
		INFO("complex as list");
		Atom a(1.7);
		REQUIRE(a.asList() == std::list<Atom>{});
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
		INFO("complex as list");
		Atom a(complex(0, 1));
		REQUIRE(a.asList() == std::list<Atom>{});
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
		INFO("symbol as list");
		Atom a("A");
		REQUIRE(a.asList() == std::list<Atom>{});
	}

	{
		INFO("list as number");
		Atom a(std::list<Atom>{Atom(1)});
		REQUIRE(a.asNumber() == 0);
	}

	{
		INFO("list as complex");
		Atom a(std::list<Atom>{Atom(1)});
		REQUIRE(a.asComplex() == complex(0, 0));
	}

	{
		INFO("list as symbol");
		Atom a(std::list<Atom>{Atom(1)});
		REQUIRE(a.asSymbol() == "");
	}

	{
		INFO("list as list");
		Atom a(std::list<Atom>{Atom(1)});
		REQUIRE(a.asList() == std::list<Atom>{Atom(1)});
	}
}
