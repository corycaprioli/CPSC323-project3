#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <ctype.h>

using namespace std;

class Lex
{
public:
	//constructor
	Lex();

	//finite state machine for integer
	int int_DFSM(const string str);

	//finite state machine for real
	int real_DFSM(const string str);

	//finite state machine for identifier
	int id_DFSM(const string str);

  int int_column(const char) const;

  int id_column(const char) const;

  int real_column(const char) const;

	//Helper functions checking separator, operator and keyword
	bool isSeparator(const char input) const;
	bool isOperator(const char input) const;
	bool checkKeyword(string identifier) const;

	//function classify a string into a specific group
	int Classify(string);

	//function returns a token and a lexeme
	void lexer(ifstream& file);

	//mutators
	void setToken(const string newToken);
	void setLexeme(const string newLexeme);

	//accessors
	string getToken() const;
	string getLexeme() const;

	//destructor
	~Lex();
protected:
	char input;
	string lexeme;
	string token;
	static int lineNum;
};

#endif

