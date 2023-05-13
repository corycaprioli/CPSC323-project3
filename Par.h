#ifndef PAR_H
#define PAR_H

#include "Lex.h"
#include <vector>
#include <utility>
#include <algorithm>
#include <stack>

struct sym_elem
{
	string id;
	int mem_loc;
	string idType;
};

struct instr_elem {
	int address;
	string op;
	int oprnd;
};

class Par : public Lex
{

public:
	//constructor
	Par();

	//syntax rule functions after removing left recursive and backtracking

	//<Rat23S> -> <Opt Function Definitions> #  <Opt Declaration List> # <Statement List>
	void RAT23S(ifstream& infile, ofstream& outfile);

	//<Qualifier> -> int   |  bool  |  real
	void Qualifier(ifstream& infile, ofstream& outfile);

	//<Opt Declaration List> -> <Declaration List>   | <Empty>
	void OptDeclarationList(ifstream& infile, ofstream& outfile);

	//<Declaration List>  -> <Declaration> ;  <Declaration List Prime>
	void DeclarationList(ifstream& infile, ofstream& outfile);

	//<Declaration List Prime>  -> <Declaration List> | <Empty>
	void DeclarationListPrime(ifstream& infile, ofstream& outfile);

	//<Declaration> ->  <Qualifier > <IDs>
	void Declaration(ifstream& infile, ofstream& outfile);

	//<IDs> ->  <Identifier> <IDs Prime>
	void IDs(ifstream& infile, ofstream& outfile);

	//<IDs Prime> ->  , <IDs> | <Empty>
	void IDsPrime(ifstream& infile, ofstream& outfile);

	//<Statement List> -> <Statement> | <Statement> <Statement List Prime>
	void StatementList(ifstream& infile, ofstream& outfile);

	//<Statement List Prime> ->  <Statement List> | <Empty>
	void StatementListPrime(ifstream& infile, ofstream& outfile);

	/*<Statement> ->  <Compound> | <Assign> | <If> |  <Return> | <Print> | <Scan> | <While> */
	void Statement(ifstream& infile, ofstream& outfile);

	//<Compound> -> {  <Statement List>  }
	void Compound(ifstream& infile, ofstream& outfile);

	//<Assign> -> <Identifier> = <Expression>;
	void Assign(ifstream& infile, ofstream& outfile);

	//<If> -> if  ( <Condition>  ) <Statement> <If Prime>
	void If(ifstream& infile, ofstream& outfile);

	//<If Prime> ->  fi | else <Statement> fi
	void IfPrime(ifstream& infile, ofstream& outfile);

	//<Return> ->  return <Return Prime>
	void Return(ifstream& infile, ofstream& outfile);

	//<Return Prime> ->  ; |  <Expression> ;
	void ReturnPrime(ifstream& infile, ofstream& outfile);

	//<Print> ->   put ( <Expression>);
	void Print(ifstream& infile, ofstream& outfile);

	//<Scan> -> get ( <IDs> );
	void Scan(ifstream& infile, ofstream& outfile);

	//<While> -> while ( <Condition>  )  <Statement> endwhile
	void While(ifstream& infile, ofstream& outfile);

	//<Condition> -> <Expression>  <Relop>   <Expression>
	void Condition(ifstream& infile, ofstream& outfile);

	//<Relop> ->   == |  !=  |   >   | <   |  =>   | <=
	void Relop(ifstream& infile, ofstream& outfile);

	//<Expression>  -> <Term> <Expression Prime>
	void Expression(ifstream& infile, ofstream& outfile);

	/* <Expression Prime>  -> + <Term> <Expression Prime>  |
	- <Term> <Expression Prime> | <Empty> */
	void ExpressionPrime(ifstream& infile, ofstream& outfile);

	//<Term>  ->  <Factor> <Term Prime>
	void Term(ifstream& infile, ofstream& outfile);

	/* <Term Prime> ->  * <Factor> <Term Prime> |  / <Factor> <Term Prime> | <Empty> */
	void TermPrime(ifstream& infile, ofstream& outfile);

	//<Factor> -> - <Primary>   | <Primary>
	void Factor(ifstream& infile, ofstream& outfile);

	//<Primary> -> <Identifier> <Primary Prime> | <Integer> | ( <Expression> ) | <Real>  | true | false 
	void Primary(ifstream& infile, ofstream& outfile);

  //<Primary Prime> -> [<IDs>] | <Empty>
	void PrimaryPrime(ifstream& infile, ofstream& outfile);

	//<Empty> -> epsilon
	void Empty(ifstream& infile, ofstream& outfile);

	//Helper functions
	void printError(ofstream& outfile);
	void print(ofstream& outfile);

	bool check_sym(string lexeme);
	void gen_sym(string lexeme, string id_type);
	void printSym(ifstream& infile, ostream& outfile);

	//Instruction Table
	int get_address(string save) const;
	void backPatch(int instr_address);
	void gen_instr(string op, int oprnd);
	void printInstr(ifstream& infile, ostream& outfile);
	string getType(string input) const;
	void checkTypeMatch(string preLexeme, string lexeme, 
		ifstream& infile, ostream& outfile);

	//Destructor
	~Par();

private:
	sym_elem sym_table[30];
	instr_elem  instr_table[1000];
	int sym_idx;
	stack <int> jumpstack;
	int instr_idx;
	string current_type;
};

#endif

