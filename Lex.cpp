#include "Lex.h"

int Lex::lineNum = 1;

//constructor
Lex::Lex()
{
	input = 'c';
}

//Function check separator
bool Lex::isSeparator(const char input) const
{
	int separators[11] = {'(', ')', '{', '}', '[', ']', ';', ':', ',', '"', '#'};
	for (int i = 0; i < 11; i++) {
		if (separators[i] == input) {
			return 1;
		}
	}
	return 0;
}
//Function check Operator
bool Lex::isOperator(const char input) const
{
	int operators[14] = {'+', '-', '*', '/', '%', '=', '>', '<', '!', '&', '|', '^', '~'};
	for (int i = 0; i < 14; i++) {
		if (operators[i] == input) {
			return 1;
		}
	}
	return 0;
}

//Function check keyword
bool Lex::checkKeyword(string identifier) const
{
	string keywords[14] = {"if", "fi", "for", "while", "int", "real", "bool", "endwhile", "put", "else", "return", "true", "false", "get"};
	for (int i = 0; i < 14; i++) {
		if (keywords[i] == identifier) {
			return 1;
		}
	}
	return 0;
}

int Lex::Classify(string s) {
	int len = s.length();

	//detect is operator or not
	for (int i = 0; i < len; i++)
	{
		if (isOperator(s[i]))
			return 1;
	}


	//detect is seperator or not
	for (int i = 0; i < len; i++)
	{
		if (isSeparator(s[i]))
			return 2;
	}

	//detect is identifier or not
	char classify_ch = s[0];
	if (isalpha(classify_ch))
	{
		for (int i = 0; i < len; i++)
		{
			//if char is either # or letter, keep checking the string sequence
			//else return 6 which is invalid input
			if (s[i] == '_' || isalpha(s[i]) || isdigit(s[i]));
			else
				return 6; //invalid input
		}
		return 3; 
	}
	else if (isdigit(classify_ch))
	{
		//first, check for valid input for real or integer
		//only accept the string with number or dot(.) sign
		for (int i = 0; i < len; i++)
		{
			if (s[i] == '.' || isdigit(s[i]));
			else
				return 6;
		}

		//second, check if there is a dot, then string could be a real number
		for (int i = 0; i < len; i++)
		{
			if (s[i] == '.')
				return 4;
		}

		//detect is integer
		return 5;
	}
	else
		return 6; //invalid input

	//not all path control above return a value. That's why we need return random number here
	return 7;
}

   int Lex::int_column(char input) const {
     if(isdigit(input)) {
      return 1; //digit column
    } else {
      return 2; //invalid
     }
  }

 int Lex::real_column(char input) const {
     if(isdigit(input)) {
      return 1; //digit column
     } else if(input == '.') {
      return 2; // '.' column
     } else {
      return 3; //invalid
     }
  }

int Lex::id_column(char input) const {
    if(isalpha(input)) {
      return 1; //letter column
    } else if(isdigit(input)) {
      return 2; //digit column
    } else if(input == '_') {
      return 3; // '_' column
    } else {
      return 4; //invalid
    }
  }

//Finite State Machine for integer
int Lex::int_DFSM(const string str)
{
	//starting state
	int state = 1;

	//create table N for the transitions or DFSM table for integer
	int a[3][2] = { 0, 'd',
                   1, 2,
                   2, 2 };

	//accepting states
	int f[1] = { 2 };

	//update the currentState to new transition
	int size = str.size();
	for (int i = 0; i < size; i++)
	{
		//convert the char to column number in table
		int col = int_column(str[i]);

		//update the current state
		state = a[state][col];
	}
	if (state == f[0])
		return 1;
	else
		return 0;
}

//Finite State Machine for floating
int Lex::real_DFSM(string str)
{
	//starting state
	int state = 1;

	int a[5][3] = { 0, 'd', '.',
                  1, 2, 0,
                  2, 2, 3,
                  3, 4, 0,
                  4, 4, 0 };

	int f[1] = { 4 };

	//convert character to its column number in the table
	int size = str.size();
	for (int i = 0; i < size; i++)
	{
		int col = real_column(str[i]);
		state = a[state][col];

		//if state = 0, then it is a failing state. Reject immediately
		if (state == 0) 
			return 0;
	}
	if (state == f[0])
		return 1;
	else
		return 0;
}

//Finite State Machine for identifier
int Lex::id_DFSM(string str)
{
	//starting state
	int state = 1;

	int a[7][4] = { 0,'l','d','_',
                  1, 2, 6, 6,
                  2, 5, 4, 3,
                  3, 5, 4, 3,
                  4, 5, 4, 3,
                  5, 5, 4, 3,
                  6, 6, 6, 6 };

	int f[4] = { 2, 3, 4, 5 };


	int size = str.size();
	for (int i = 0; i < size; i++)
	{
		int col = id_column(str[i]);
		state = a[state][col];
		if (state == 0)
			return 0;
	}

	for (int i = 0; i < 4; i++)
	{
		if (state == f[i])
			return 1;
	}
	return 0;
}

void Lex::lexer(ifstream& file)
{
	string str; 
	int state_status = 0;
	bool found = false;
	char ch = 'c', prevChar = 'c';

	while (!found)
	{
		if (prevChar == '\n')
			lineNum++;
		ch = file.get();
		prevChar = ch;

		if (this->isSeparator(ch) || this->isOperator(ch) || isspace(ch) || ch == -1)
		{
			found = true;
		}

		if (!str.empty() && (this->isOperator(ch) || this->isSeparator(ch))) {
			file.unget();
    }
		else if (!isspace(ch) && !(ch == -1)) {
			str += ch;
    }

		if (str.empty() && !(ch == -1)) {
			found = false;
    }
	}

	if (str.empty() && ch == -1)
	{
		this->setLexeme("EOF");
		this->setToken("EOF");
		return;
	}

	int classify = Classify(str);

	//check token using FSM for identifier
	if (classify == 3) {

		state_status = id_DFSM(str);

		this->setLexeme(str);
		if (state_status == 1)
		{
			if (checkKeyword(str)) {
				this->setToken("keyword");
      } else {
				this->setToken("identifier");
      }
		} else {
			this->setToken("invalid identifier");
		}
	}

	else if (classify == 1)
	{
		str = ch;

		ch = file.peek();

		if ((str[0] == '=' && ch == '=') ||
        (str[0] == '!' && ch == '=') ||
        (str[0] == '>' && ch == '=') ||
        (str[0] == '<' && ch == '='))
		{
			str += ch;
			file.get();
		}

		if (str == "/=" ||
        str == "==" ||
        str == "!=" ||
        str == ">=" ||
        str == "<=" ||
        isOperator(str[0]))
		{
			this->setToken("operator");
			this->setLexeme(str);
		}
		else
		{
			this->setToken("invalid operator");
			this->setLexeme(str);
		}
	}
	//check for separator
	else if (classify == 2) {
		str = ch;
		ch = file.peek();

		if (!(str[0] == '#'))
		{
			this->setLexeme(str);
			this->setToken("separator");
		}
		else
		{
			this->setLexeme(str);
			this->setToken("invalid separator");
		}
	}
	//check token using FSM for real
	else if (classify == 4) {
		state_status = real_DFSM(str);
		this->setLexeme(str);
		if (state_status == 1)
		{
			this->setToken("real");
		}
		else {
			this->setToken("invalid real");
		}
	}
	//check token using FSM for floating
	else if (classify == 5)
	{
		state_status = int_DFSM(str);
		this->setLexeme(str);

		if (state_status == 1)
		{
			this->setToken("integer");
		}
		else
			this->setToken("invalid integer");
	}
	else
	{
		this->setLexeme(str);
		this->setToken("invalid input");
	}
}

void Lex::setToken(const string newToken)
{
	token = newToken;
}

void Lex::setLexeme(const string newLexeme)
{
	lexeme = newLexeme;
}

string Lex::getToken() const
{
	return token;
}

string Lex::getLexeme() const
{
	return lexeme;
}

//destructor
Lex::~Lex() {}
