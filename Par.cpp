#include "Par.h"
#include <iostream>
#include <string>
#define BLANK -9999

using namespace std;

int memory_address = 10000;
bool isFromRead = false;
int count_sym = 0;
bool isFromDeclaration = false;
bool dummyLabel = false;
string prevLexeme;
string temp;

//constructor
Par::Par()
{
	sym_idx = 0;
	sym_table[sym_idx].mem_loc = 0;
	instr_idx = 1;
}

bool Par::check_sym(string lexeme)
{
	for (int i = 0; i < sym_idx; i++)
	{
		if (lexeme == sym_table[i].id) {
			if (isFromDeclaration == true)
				count_sym++;
			return true;
		}

	}
	return false;
}

void Par::gen_sym(string lexeme, string id_type)
{
	sym_table[sym_idx].id = lexeme;
	sym_table[sym_idx].mem_loc = memory_address;
	sym_table[sym_idx].idType = id_type;
	sym_idx++;
	memory_address++;
}

void Par::printSym(ifstream& infile, ostream& outfile)
{
	outfile << "\nSYMBOL TABLE\n";
	for (int i = 0; i < sym_idx; i++)
	{
		outfile << sym_table[i].id << " " << sym_table[i].mem_loc << " " << sym_table[i].idType;
		outfile << endl;
	}
}

int Par::get_address(string save) const
{
	int address = 0;
	for (int i = 0; i < sym_idx; i++) {
		if (sym_table[i].id == save) {
			address = sym_table[i].mem_loc;
		}
	}
	return address;
}

void Par::backPatch(int jump_addr)
{
	int addr = jumpstack.top();
	jumpstack.pop();
	instr_table[addr].oprnd = jump_addr;
}

void Par::gen_instr(string op, int oprnd)
{
	instr_table[instr_idx].address = instr_idx;
	instr_table[instr_idx].op = op;
	instr_table[instr_idx].oprnd = oprnd;
	instr_idx++;
}

void Par::printInstr(ifstream& infile, ostream& outfile)
{
	string a;
	outfile << "\nINSTRUCTION TABLE\n";
	for (int i = 1; i < instr_idx; i++)
	{
		if (instr_table[i].oprnd == BLANK) {
			a = "";
			outfile << instr_table[i].address << " " << instr_table[i].op << " " << a;
			outfile << endl;
		}
		else {
			outfile << instr_table[i].address << " " << instr_table[i].op << " " << instr_table[i].oprnd;
			outfile << endl;
		}

	}
}

string Par::getType(string input) const
{
	string a = "";
	for (int i = 0; i < sym_idx; i++) {
		if (sym_table[i].id == input) {
			a = sym_table[i].idType;
		}
	}

	if (input == "true" || input == "false") {
		a = "bool";
	}
	return a;

}

/*
Check Type of current lexeme vs previous lexeme
if does not match, print error
*/
void Par::checkTypeMatch(string prevLexeme, string lexeme,
	ifstream& infile, ostream& outfile)
{
	if (getType(prevLexeme) == "bool" || getType(lexeme) == "bool") {
		outfile << "No Arithmetic operations are allowed for bool." << endl;
	}
}

//Print token and lexeme
void Par::print(ofstream& outfile)
{
		outfile << "Token: " << left << setw(20) << this->token
			<< "Lexeme: " << setw(20) << this->lexeme << endl;
}

//syntax rule functions
void Par::RAT23S(ifstream& infile, ofstream& outfile)
{
	//Get the first token in file.txt
	lexer(infile);
	print(outfile);
		outfile << "\t<RAT23S> -> # <Opt Declaration List> # <Statement List>\n";
	if (lexeme == "#")
	{
		lexer(infile);
		print(outfile);
		OptDeclarationList(infile, outfile);
    if(lexeme == "#") {
      lexer(infile);
      print(outfile);
		  StatementList(infile, outfile);
    } else {
      outfile << "\n expected '#' \n";
    }

		//reset the line number
		lineNum = 1;
	}
	else
	{
		printError(outfile);
		outfile << "\n expected '#' \n";
	}
}

void Par::Qualifier(ifstream& infile, ofstream& outfile)
{
	if (token == "keyword" && lexeme == "int")
	{
		current_type = lexeme;
		outfile << "\t<Qualifier> -> int\n";
		lexer(infile);
		print(outfile);
	}
	else if (token == "keyword" && lexeme == "bool")
	{
		current_type = lexeme;
			outfile << "\t<Qualifier> -> bool\n";
		lexer(infile);
		print(outfile);
	}
	else
	{
		printError(outfile);
		outfile << "Invalid keyword\n";
	}
}

void Par::OptDeclarationList(ifstream& infile, ofstream& outfile)
{
	if (lexeme == "int" || lexeme == "bool")
	{

			outfile << "\t<Opt Declaration List> -> <Declaration List>\n";
		DeclarationList(infile, outfile);
	}
	else
	{
			outfile << "\t<Opt Declaration List> -> Epsilon" << endl;
		Empty(infile, outfile);
	}
}

void Par::DeclarationList(ifstream& infile, ofstream& outfile)
{
	isFromDeclaration = true;
		outfile << "\t<Declaration List> -> <Declaration> ; <Declaration List Prime>\n";
	Declaration(infile, outfile);
	if (lexeme == ";")
	{
		lexer(infile);
		print(outfile);
		DeclarationListPrime(infile, outfile);
	}
	else
	{
		printError(outfile);
		outfile << "';' is expected \n";
	}
	isFromDeclaration = false;
}

void Par::DeclarationListPrime(ifstream& infile, ofstream& outfile)
{
	if (lexeme == "int" || lexeme == "bool")
	{
			outfile << "\t<Declaration List Prime> -> <Declaration List>\n";
		DeclarationList(infile, outfile);
	}
	else
	{
			outfile << "\t<Declaration List Prime> -> " << "Epsilon" << endl;
		Empty(infile, outfile);
	}
}

void Par::Declaration(ifstream& infile, ofstream& outfile)
{
		outfile << "\t<Declaration> -> <Qualifier> <IDs>\n";
	Qualifier(infile, outfile);
	IDs(infile, outfile);
}

void Par::IDs(ifstream& infile, ofstream& outfile)
{
	if (token == "identifier")
	{
		if (!check_sym(lexeme) && isFromRead == false)
		{
			gen_sym(lexeme, current_type);
		}
		else if (!check_sym(lexeme) && isFromRead == true)
		{
			outfile << "Identifier " << lexeme << " has not been declared yet.\n";
		}
		if (count_sym == 2) {
			outfile << "Identifier " << lexeme << " is already declared.\n";
		}

			outfile << "\t<IDs> -> <identifier> <IDs Prime>\n";
		lexer(infile);
		print(outfile);
		IDsPrime(infile, outfile);
	}
	else
	{
		printError(outfile);
		outfile << "\n<identifier> is expected\n";
	}
}

void Par::IDsPrime(ifstream& infile, ofstream& outfile)
{
	if (lexeme == ",")
	{
			outfile << "\t<IDsPrime> -> , <IDs>\n";
		lexer(infile);
		print(outfile);
		IDs(infile, outfile);
	}
	else
	{
			outfile << "\t<IDs Prime> -> Epsilon" << endl;
		Empty(infile, outfile);
	}
}

void Par::StatementList(ifstream& infile, ofstream& outfile)
{
		outfile << "\t<Statement List> -> <Statement> <Statement List Prime>\n";
	Statement(infile, outfile);
	StatementListPrime(infile, outfile);
}

void Par::StatementListPrime(ifstream& infile, ofstream& outfile)
{
	if (lexeme == "{" || token == "identifier" || lexeme == "if" || lexeme == "return" || lexeme == "print" || lexeme == "scan" || lexeme == "while")
	{
			outfile << "\t<Statement List Prime> -> <Statement List>\n";
		StatementList(infile, outfile);
	}
	else
	{
		if (!(lexeme == "EOF"))
		{
			outfile << "\t<Statement List Prime> -> Epsilon" << endl;
		}
		Empty(infile, outfile);
	}
}

void Par::Statement(ifstream& infile, ofstream& outfile)
{
	if (lexeme == "{")
	{
			outfile << "\t<Statement> -> <Compound>\n";
		Compound(infile, outfile);
	}
	else if (token == "identifier")
	{
			outfile << "\t<Statement> -> <Assign>\n";
		Assign(infile, outfile);
	}
	else if (lexeme == "if")
	{
			outfile << "\t<Statement> -> <If>\n";
		If(infile, outfile);
	}
	else if (lexeme == "return")
	{
			outfile << "\t<Statement> -> <Return>\n";
		Return(infile, outfile);
	}
	else if (lexeme == "print")
	{
			outfile << "\t<Statement> -> <Print>\n";
		Print(infile, outfile);
	}
	else if (lexeme == "read")
	{
			outfile << "\t<Statement> -> <Scan>\n";
		Scan(infile, outfile);
	}
	else if (lexeme == "while")
	{
			outfile << "\t<Statement> -> <While>\n";
		While(infile, outfile);
	}
	else
	{
		printError(outfile);
		outfile << "'compound', 'if', 'return', 'print', 'scan','while' keyword expects\n";
	}
}

void Par::Compound(ifstream& infile, ofstream& outfile)
{
	if (lexeme == "{")
	{
			outfile << "\t<Compound> -> { <StatementList> }\n";
		lexer(infile);
		print(outfile);
		StatementList(infile, outfile);
		if (lexeme == "}")
		{
			lexer(infile);
			print(outfile);
		}
		else
		{
			printError(outfile);
			outfile << "\n '}' is expected\n";
		}
	}
	else
	{
		printError(outfile);
		outfile << "\n'{' is expected\n";
	}
}

void Par::Assign(ifstream& infile, ofstream& outfile)
{
	if (token == "identifier")
	{
		if (!check_sym(lexeme))
		{
			outfile << "Identifier " << lexeme << " has not been declared yet.\n";
		}
			outfile << "\t<Assign> -> <Identifier> = <Expression>;\n";
		string save = lexeme;
		temp = lexeme;
		lexer(infile);
		print(outfile);
		if (lexeme == "=")
		{
			lexer(infile);
			print(outfile);
			Expression(infile, outfile);
			int addr = get_address(save);
			gen_instr("POPM", addr);
			if (lexeme == ";")
			{
				lexer(infile);
				print(outfile);
			}
			else
			{
				printError(outfile);
				outfile << "\n ';' is expected\n";
			}
		}
		else
		{
			printError(outfile);
			outfile << "\n'=' is expected\n";
		}
	}
	else
	{
		printError(outfile);
		outfile << "\n <identifier> is expected\n";
	}
}

void Par::If(ifstream& infile, ofstream& outfile)
{
	if (lexeme == "if")
	{
			outfile << "\t<If> -> if (<Condition>) <Statement> <If Prime>\n";
		lexer(infile);
		print(outfile);
		if (lexeme == "(")
		{
			lexer(infile);
			print(outfile);
			Condition(infile, outfile);
			if (lexeme == ")")
			{
				lexer(infile);
				print(outfile);
				Statement(infile, outfile);
				if (lexeme == "else")
				{
					int addr = jumpstack.top();
					jumpstack.pop();
					jumpstack.push(instr_idx);
					gen_instr("JUMP", BLANK);

					jumpstack.push(addr);
					dummyLabel = true;
				}
				backPatch(instr_idx);

				IfPrime(infile, outfile);
				if (dummyLabel == true)
					backPatch(instr_idx);
				if (lexeme == "EOF")
					gen_instr("LABEL", BLANK);
			}
			else
			{
				printError(outfile);
				outfile << "\n')' is expected\n";
			}
		}
		else
		{
			printError(outfile);
			outfile << "\n'(' is expected\n";
		}
	}
	else
	{
		printError(outfile);
		outfile << "\n 'if' keyword is expected\n";
	}
}

void Par::IfPrime(ifstream& infile, ofstream& outfile)
{
	if (lexeme == "fi")
	{
			outfile << "\t<If Prime> -> fi\n";
		lexer(infile);
		print(outfile);
	}
	else if (lexeme == "else")
	{
			outfile << "\t<If Prime> -> else <Statement> fi\n";
		lexer(infile);
		print(outfile);
		Statement(infile, outfile);
		if (lexeme == "fi")
		{
			lexer(infile);
			print(outfile);
		}
		else
		{
			printError(outfile);
			outfile << "\n'fi' is expected\n";
		}
	}
	else
	{
		printError(outfile);
		outfile << "Invalid keyword, 'fi' is expected or 'else' is expected\n";
	}
}

void Par::Return(ifstream& infile, ofstream& outfile)
{
	if (lexeme == "return")
	{
			outfile << "\t<Return> -> return <Return Prime>\n";
		lexer(infile);
		print(outfile);
		ReturnPrime(infile, outfile);
	}
	else
	{
		printError(outfile);
		outfile << "\n'return' is expected\n";
	}
}

void Par::ReturnPrime(ifstream& infile, ofstream& outfile)
{
	if (lexeme == ";")
	{
			outfile << "\t<Return Prime> -> ;\n";
		lexer(infile);
		print(outfile);
	}
	else
	{
			outfile << "\t<Return Prime> -> <Expression>;\n";
		Expression(infile, outfile);
		if (lexeme == ";")
		{
			lexer(infile);
			print(outfile);
		}
		else
		{
			printError(outfile);
			outfile << "\n ';' is expected.\n";
		}
	}
}

void Par::Print(ifstream& infile, ofstream& outfile)
{
	if (lexeme == "put")
	{
			outfile << "\t<Write> -> put (<Expression>);\n";
		lexer(infile);
		print(outfile);
		if (lexeme == "(")
		{
			lexer(infile);
			print(outfile);
			Expression(infile, outfile);
			if (lexeme == ")")
			{
				gen_instr("STDOUT", BLANK);
				lexer(infile);
				print(outfile);
				if (lexeme == ";")
				{
					lexer(infile);
					print(outfile);
				}
				else
				{
					printError(outfile);
					outfile << "\n';' is expected.\n";
				}
			}
			else
			{
				printError(outfile);
				outfile << "\n ')' is expected\n";
			}
		}
		else
		{
			printError(outfile);
			outfile << "\n '(' is expected.\n";
		}
	}
	else
	{
		printError(outfile);
		outfile << "\n 'put' is expected.\n";
	}
}

void Par::Scan(ifstream& infile, ofstream& outfile)
{
	isFromRead = true;
	if (lexeme == "get")
	{
			outfile << "\t<Scan> -> get (<IDs>);\n";
		lexer(infile);
		print(outfile);
		if (lexeme == "(")
		{
			lexer(infile);
			string save = lexeme;
			print(outfile);
			IDs(infile, outfile);
			if (lexeme == ")")
			{
				int addr = get_address(save);
				gen_instr("STDIN", BLANK);
				gen_instr("POPM", addr);
				lexer(infile);
				print(outfile);
				if (lexeme == ";")
				{
					lexer(infile);
					print(outfile);
				}
				else
				{
					printError(outfile);
					outfile << "\n ';' is expected.\n";
				}
			}
			else
			{
				printError(outfile);
				outfile << "\n ')' is expected.\n";
			}
		}
		else
		{
			printError(outfile);
			outfile << "\n '(' is expected.\n";
		}
	}
	else
	{
		printError(outfile);
		outfile << "\n 'get' is expected.\n";
	}
}

void Par::While(ifstream& infile, ofstream& outfile)
{
	if (lexeme == "while")
	{
			outfile << "\t<While> -> <while (<Condition>) <Statement>\n";
		int addr = instr_idx;
		gen_instr("LABEL", BLANK);
		lexer(infile);
		print(outfile);
		if (lexeme == "(")
		{
			lexer(infile);
			temp = lexeme;
			print(outfile);
			Condition(infile, outfile);
			if (lexeme == ")")
			{
				lexer(infile);
				Statement(infile, outfile);
				gen_instr("JUMP", addr);
				backPatch(instr_idx);
				if (lexeme == "EOF")
					gen_instr("LABEL", BLANK);
			}
			else
			{
				printError(outfile);
				outfile << "\n ')' is expected.\n";
			}
		}
		else
		{
			printError(outfile);
			outfile << "\n '(' is expected.\n";
		}
	}
	else
	{
		printError(outfile);
		outfile << "\n 'while' is expected.\n";
	}
}

void Par::Condition(ifstream& infile, ofstream& outfile)
{
		outfile << "\t<Condition> -> <Expression> <Relop> <Expression>\n";
	Expression(infile, outfile);
	string op = lexeme;
	Relop(infile, outfile);
	Expression(infile, outfile);
	if (op == "==")
	{
		gen_instr("EQ", BLANK);
		jumpstack.push(instr_idx);
		gen_instr("JUMPZ", BLANK);
	}
	else if (op == "!=")
	{
		gen_instr("NEQ", BLANK);
		jumpstack.push(instr_idx);
		gen_instr("JUMPZ", BLANK);
	}
	else if (op == ">")
	{
		gen_instr("GRT", BLANK);
		jumpstack.push(instr_idx);
		gen_instr("JUMPZ", BLANK);
	}
	else if (op == "<")
	{
		gen_instr("LES", BLANK);
		jumpstack.push(instr_idx);
		gen_instr("JUMPZ", BLANK);
	}
	else if (op == "=>")
	{
		gen_instr("GEQ", BLANK);
		jumpstack.push(instr_idx);
		gen_instr("JUMPZ", BLANK);
	}
	else if (op == "<=")
	{
		gen_instr("LEQ", BLANK);
		jumpstack.push(instr_idx);
		gen_instr("JUMPZ", BLANK);
	}
	else
	{
		printError(outfile);
		outfile << "\n '==', '!=', '>', '<', '=>' or '<=' is expected.\n";
	}
}

void Par::Relop(ifstream& infile, ofstream& outfile)
{
	if (lexeme == "==")
	{
			outfile << "\t<Relop> -> ==\n";
		lexer(infile);
		print(outfile);
	}
	else if (lexeme == "!=")
	{
			outfile << "\t<Relop> -> !=\n";
		lexer(infile);
		print(outfile);
	}
	else if (lexeme == ">")
	{
			outfile << "\t<Relop> -> >\n";
		lexer(infile);
		print(outfile);
	}
	else if (lexeme == "<")
	{
			outfile << "\t<Relop> -> <\n";
		lexer(infile);
		print(outfile);
	}
	else if (lexeme == "=>")
	{
			outfile << "\t<Relop> -> =>\n";
		lexer(infile);
		print(outfile);
	}
	else if (lexeme == "<=")
	{
			outfile << "\t<Relop> -> <=\n";
		lexer(infile);
		print(outfile);
	}
	else
	{
		printError(outfile);
		outfile << "\n '==', '!=', '>', '<', '=>' or '<=' is expected.\n";
	}
}

void Par::Expression(ifstream& infile, ofstream& outfile)
{
		outfile << "\t<Expression> -> <Term> <Expression Prime>\n";
	Term(infile, outfile);
	ExpressionPrime(infile, outfile);
}

void Par::ExpressionPrime(ifstream& infile, ofstream& outfile)
{
	if (lexeme == "+")
	{
			outfile << "\t<Expression Prime> -> + <Term> <Expression Prime>\n";
		lexer(infile);
		checkTypeMatch(prevLexeme, lexeme, infile, outfile);
		print(outfile);
		Term(infile, outfile);
		gen_instr("ADD", BLANK);
		ExpressionPrime(infile, outfile);
	}
	else if (lexeme == "-")
	{
			outfile << "\t<Expression Prime> -> - <Term> <Expression Prime>\n";
		lexer(infile);
		checkTypeMatch(prevLexeme, lexeme, infile, outfile);
		print(outfile);
		Term(infile, outfile);
		gen_instr("SUB", BLANK);
		ExpressionPrime(infile, outfile);
	}
	else
	{
			outfile << "\t<Expression Prime> -> Epsilon" << endl;
		Empty(infile, outfile);
	}
}

void Par::Term(ifstream& infile, ofstream& outfile)
{
		outfile << "\t<Term> -> <Factor> <Term Prime>\n";
	Factor(infile, outfile);
	TermPrime(infile, outfile);
}

void Par::TermPrime(ifstream& infile, ofstream& outfile)
{
	if (lexeme == "*")
	{
			outfile << "\t<Term Prime> -> * <Factor> <Term Prime>\n";
		lexer(infile);
		checkTypeMatch(prevLexeme, lexeme, infile, outfile);
		print(outfile);
		Factor(infile, outfile);
		gen_instr("MUL", BLANK);
		TermPrime(infile, outfile);
	}
	else if (lexeme == "/")
	{
			outfile << "\t<Term Prime> -> / <Factor> <Term Prime>\n";
		lexer(infile);
		checkTypeMatch(prevLexeme, lexeme, infile, outfile);
		print(outfile);
		Factor(infile, outfile);
		gen_instr("DIV", BLANK);
		TermPrime(infile, outfile);
	}
	else
	{
		outfile << "\t<Term Prime> -> Epsilon" << endl;
		Empty(infile, outfile);
	}
}

void Par::Factor(ifstream& infile, ofstream& outfile)
{
	if (lexeme == "-")
	{
		outfile << "\t<Factor> -> - <Primary>\n";
		prevLexeme = lexeme;
		lexer(infile);
		print(outfile);
		Primary(infile, outfile);
	}
	else
	{
		outfile << "\t<Factor> -> <Primary>\n";
		Primary(infile, outfile);
	}
}

void Par::Primary(ifstream& infile, ofstream& outfile)
{
	if (token == "identifier")
	{
		if (!check_sym(lexeme))
		{
			outfile << "Identifier " << lexeme << " has not been declared yet.\n";
		}
		if (!(getType(temp) == getType(lexeme)) && !(temp == "")) {
			outfile << "The type of " << temp << " and " << lexeme << " must match" << endl;
		}
			outfile << "\t<Primary> -> <identifier> <Primary Prime>\n";
		int addr = get_address(lexeme);
		gen_instr("PUSHM", addr);
		//prevLexeme = lexeme;
		lexer(infile);
		print(outfile);
		PrimaryPrime(infile, outfile);
	}
	else if (token == "int")
	{
		if (!(getType(temp) == "int") && !(temp == "")) {
			outfile << "The type of " << temp << " and " << lexeme << " must match" << endl;
		}
			outfile << "\t<Primary> -> <int>\n";
		int a = 0;
		if (prevLexeme == "-")
		{
			prevLexeme = prevLexeme + lexeme;
			a = stoi(prevLexeme);
		}
		else
			a = stoi(lexeme);
		gen_instr("PUSHI", a);
		lexer(infile);
		print(outfile);
	}
	else if (lexeme == "(")
	{
			outfile << "\t<Primary> -> (<Expression>)\n";
		lexer(infile);
		print(outfile);
		Expression(infile, outfile);
		if (lexeme == ")")
		{
			lexer(infile);
			print(outfile);
		}
		else
		{
			printError(outfile);
			outfile << "\n')' is expected.\n";
		}
	}
	else if (lexeme == "true")
	{
		if (!(getType(temp) == "bool") && !(temp == "") || prevLexeme == "-") {
			if (prevLexeme == "-")
			{
				outfile << "Cannot assign " << temp << " to " << prevLexeme + lexeme << endl;
			}
			else
			{
				outfile << "The type of " << temp << " and " << lexeme << " must match" << endl;
			}
		}
			outfile << "\t<Prime> -> true\n";
		gen_instr("PUSHI", 1);
		lexer(infile);
		print(outfile);
	}
	else if (lexeme == "false")
	{
		if (!(getType(temp) == "bool") && !(temp == "") || prevLexeme == "-") {
			if (prevLexeme == "-")
			{
				outfile << "Cannot assign " << temp << " to " << prevLexeme + lexeme << endl;
				cerr << "Cannot assign " << temp << " to " << prevLexeme + lexeme << endl;
			}
			else
			{
				outfile << "The type of " << temp << " and " << lexeme << " must match"	<< endl;

			}
		}
			outfile << "\t<Prime> -> false\n";
		gen_instr("PUSHI", 0);
		lexer(infile);
		print(outfile);
	}
	else
	{
		printError(outfile);
		outfile << "<identifier>, <integer>, '(', 'true' or 'false' is expected.\n";
	}
}

void Par::PrimaryPrime(ifstream& infile, ofstream& outfile)
{
	if (lexeme == "[")
	{
		outfile << "\t<Primary Prime> -> [<IDs>]\n";
		lexer(infile);
		print(outfile);
		IDs(infile, outfile);
		if (lexeme == "]")
		{
			lexer(infile);
			print(outfile);
		}
		else
		{
			printError(outfile);
			outfile << "\n']' is expected.\n";
		}
	}
	else
	{
		outfile << "\t<Primary Prime> -> Epsilon" << endl;
		Empty(infile, outfile);
	}
}

void Par::Empty(ifstream& infile, ofstream& outfile)
{
	//Do nothing
}

void Par::printError(ofstream& outfile)
{
	outfile << "Error at line " << lineNum << endl;

	lineNum = 1;
}

//Destructor
Par::~Par()
{
	memory_address = 10000;
	count_sym = 0;
	isFromRead = false;
	isFromDeclaration = false;
	dummyLabel = false;
	prevLexeme = "";
	temp = "";
}
