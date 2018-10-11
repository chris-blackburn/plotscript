#include "token.hpp"

// system includes
#include <cctype>
#include <iostream>

// define constants for special characters
#define OPENCHAR '('
#define CLOSECHAR ')'
#define COMMENTCHAR ';'
#define QUOTECHAR '"'

Token::Token(TokenType t): m_type(t) {}

Token::Token(const std::string& str): m_type(STRING), value(str) {}

Token::TokenType Token::type() const {
	return m_type;
}

std::string Token::asString() const {
	switch(m_type) {
	case OPEN:
		return "(";
	case CLOSE:
		return ")";
	case STRING:
		return value;
	}

	return "";
}


// add token to sequence unless it is empty, clears token
void store_ifnot_empty(std::string& token, TokenSequenceType& seq) {
	if (!token.empty()) {
		seq.emplace_back(token);
		token.clear();
	}
}

TokenSequenceType tokenize(std::istream& seq) {
	TokenSequenceType tokens;
	std::string token;

	// For keeping track of when you exclude spaces
	bool openQuote = false;
	while (true) {
		char c = seq.get();
		if (seq.eof()) {
			break;
		}

		if (c == COMMENTCHAR) {

			// chomp until the end of the line
			while (!seq.eof() && c != '\n') {
				c = seq.get();
			}

			if(seq.eof()) {
				break;
			}
		} else if (c == OPENCHAR) {
			store_ifnot_empty(token, tokens);
			tokens.push_back(Token::TokenType::OPEN);
		} else if(c == CLOSECHAR) {
			store_ifnot_empty(token, tokens);
			tokens.push_back(Token::TokenType::CLOSE);
			store_ifnot_empty(token, tokens);
		} else if (c == QUOTECHAR) {
			token.push_back(c);

			// If we are at a close quote, then store the result. This gets checked
			// before updating the boolean since it will be true when it hits a
			// closing quote
			if (openQuote) {
				store_ifnot_empty(token, tokens);
			}

			// Toggle boolean to keep track of quotes
			openQuote = !openQuote;
		} else if(isspace(c)) {

			// Check if it is specifically whitespace inside quotes
			if (c == 0x20 && openQuote) {
				token.push_back(c);
			} else {
				store_ifnot_empty(token, tokens);
			}
		} else {
			token.push_back(c);
		}
	}

	store_ifnot_empty(token, tokens);
	return tokens;
}
