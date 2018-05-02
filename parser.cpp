//#include "stdafx.h"
#include "parser.h"
#include "lexer.h"
#include <fstream>
#include "globals.h"
#include <stack>
#include <vector>
#include <map>
#include <iomanip>      // std::setw
#include <string>
using namespace std;

vector<string> allWords;
vector<Symbol> symbolTable;	//stores token.value and memAddr
vector<Instr> instrTable;	//stores
vector<int> jumpVect;

stack<int> jumpStack;


int tokenIndex;
int memAddr = 1999;
int ruleNum;
int instr_address = 1; // global variable for instruction address
Token token;
string symbolType;

void Parser(vector<string> v) {
	allWords = v;
	tokenIndex = 0;
	token = lexer(allWords.at(0));
	Rat18S();
}

void Error() {
	fstream coutfile(outputFile, ios_base::app);
	coutfile << "Syntax Error!" << endl;
	coutfile.close();
	exit(0);
}

void PrintToken(bool doit) {
	doit = false;
    if (doit) {
		fstream coutfile(outputFile, ios_base::app);
		coutfile << endl << token.type << "\t\t" << token.value << endl;
		doit = false;
		coutfile.close();
	}
}

void printSymbol() {
	fstream coutfile(outputFile, ios_base::app);
	coutfile.setf(ios::left);
	coutfile << "\n\t\tSymbol Table" << endl;
	coutfile << setw(10) << "ID" << setw(20) << "Type" << setw(10) << "MemLoc" << endl;
	coutfile.close();
	for (int i = 0; i < symbolTable.size(); ++i) {
		fstream coutfile(outputFile, ios_base::app);
		coutfile.setf(ios::left);
		coutfile << setw(10) << symbolTable.at(i).name << setw(20) << symbolTable.at(i).type << setw(10)
			<< symbolTable.at(i).addr << endl;
		coutfile.close();
	}
}

void printInstr() {
	fstream coutfile(outputFile, ios_base::app);
	coutfile.setf(ios::left);
	coutfile << "\n\t\tInstr Table" << endl;
	coutfile << setw(10) << "Address" << setw(20) << "Op" << setw(10) << "Oprnd" << endl;
	coutfile.close();
	for (int i = 0; i < instrTable.size(); ++i) {
		fstream coutfile(outputFile, ios_base::app);
		coutfile.setf(ios::left);
		coutfile << setw(10) << instrTable.at(i).addr << setw(20) << instrTable.at(i).optor << setw(10)
			<< instrTable.at(i).oprand << endl;
		coutfile.close();
	}
}

Pair checkID() {
	Pair p;
	p.declared = false;
	p.symbolIndex = -1;
	if (token.type == "identifier") {
		//check an identifier is declared or not
		for (int i = 0; i < symbolTable.size(); ++i) {
			if (symbolTable.at(i).name == token.value) {
				p.declared = true;
				p.symbolIndex = i;
			}
		}
	}
	return p;
}

void NextToken() {
	if (tokenIndex < allWords.size()) {
		token = lexer(allWords.at(++tokenIndex));
		PrintToken(true);
	}
}

//old : R1: <Rat18S> → <Opt Function Definitions> %% <Opt Declaration List> <Statement List>
//new:  R1: <Rat18S> → %% <Opt Declaration List> <Statement List>
void Rat18S() {
	PrintToken(true);
	PrintRule(1);

	if (token.value == "%%") {
		NextToken();
		OptDeclarationList();
		StatementList();
	}
	else Error();

	string lastWord = allWords.back();
	if (lastWord != "$") Error();
	else {
		fstream coutfile(outputFile, ios_base::app);
		coutfile.setf(ios::left);
		coutfile << "\nFinished Parsing!" << endl;
		coutfile.close();
	}
}


//R6: <Opt Parameter List> → <Parameter List> | <Empty>
void OptParameterList() {
	PrintRule(6);
	if (token.type == "identifier") {
		ParameterList();
	}
	else
		Empty();
}

//R7: <Parameter List> → <Parameter> <Parameter List’>
void ParameterList() {
	PrintRule(7);
	Parameter();
	ParameterListP();
}

//R8: <Parameter List’> → , <Parameter List> |  𝜀
void ParameterListP() {
	PrintRule(8);
	if (token.type == "NotToken" || token.type == "COMMENT") {
		Empty();
	}
	if (token.value == ",") {
		NextToken();
		ParameterList();
	}
}

//R9: <Parameter> → <IDs> : <Qualifier>
void Parameter() {
	PrintRule(9);
	IDs();
	//token = lexer(allWords.at(++tokenIndex));
	//PrintToken(true);
	if (token.value == ":") {
		NextToken();
		Qualifier();
	}
	else Error();
}

//R10: <Qualifier> → int | boolean | real
void Qualifier() {
	PrintRule(10);

	if (token.value == "int" || token.value == "boolean" || token.value == "real") {
		symbolType = token.value;
		NextToken();
	}
	else Error();
}

//R11: <Body> → { <Statement List> }
void Body() {
	PrintRule(11);
	if (token.value == "{") {
		NextToken();
		StatementList();
		if (token.value == "}") {
			NextToken();
		}
		else Error();
	}
	else Error();
}

//R12: <Opt Declaration List> → <Declaration List> | <Empty>
void OptDeclarationList() {
	PrintRule(12);
	if (token.value == "int" || token.value == "boolean" || token.value == "real") {
		DeclarationList();
	}
	else
		Empty();
}

//R13: <Declaration List> → <Declaration> ; <Declaration List’>
void DeclarationList() {
	PrintRule(13);
	if (token.value == "int" || token.value == "boolean" || token.value == "real") {
		Declaration();
		if (token.value == ";") {
			NextToken();
			DeclarationListP();
		}
		else
			Error();
	}

	else
		Error();
}

//R14: <Declaration List’> → <Declaration List> | <Empty>
void DeclarationListP() {
	PrintRule(14);

	if (token.value == "int" || token.value == "boolean" || token.value == "real") {
		DeclarationList();
	}
	else
		Empty();
}

//R15: <Declaration> → <Qualifier> <IDs>
void Declaration() {
	PrintRule(15);
	Qualifier();

	if (!checkID().declared) {
		Symbol symbol;
		++memAddr;
		symbol.addr = memAddr;
		symbol.name = token.value;
		symbol.type = symbolType;
		symbolTable.push_back(symbol);
	}
	else {
		fstream coutfile(outputFile, ios_base::app);
		coutfile << "Error! " << token.value << " is already declard!" << endl;
		coutfile.close();
		exit(0);
	}

	IDs();
}

//R16: <IDs> → <Identifier> <IDs’>
void IDs() {
	PrintRule(16);
	Identifier();
	IDsP();
}

//R17: <IDs’> → , <IDs> | <Empty>
void IDsP() {
	PrintRule(17);
	if (token.type == "NotToken" || token.type == "COMMENT" || token.value != ",") {
		Empty();
	}
	else {
		if (token.value == ",") {
			NextToken();

			if (!checkID().declared) {
				Symbol symbol;
				++memAddr;
				symbol.addr = memAddr;
				symbol.name = token.value;
				symbol.type = symbolType;
				symbolTable.push_back(symbol);
			}
			else {
				fstream coutfile(outputFile, ios_base::app);
				coutfile << "Error! " << token.value << " is already declard!" << endl;
				coutfile.close();
				exit(0);
			}

			IDs();
		}
		else Error();
	}
}

//R18: <Statement List> → <Statement> <Statement List’>
void StatementList() {
	PrintRule(18);
	Statement();
	StatementListP();
}

//R19: <Statement List’> → <Statement List> | <Empty>
void StatementListP() {
	PrintRule(19);
	if (token.type == "NotToken" || token.type == "COMMENT") {
		Empty();
	}
	else {
		StatementList();
	}

}

//R20: <Statement> → <Compound> | <Assign> | <If> | <Return> | <Print> | <Scan> | <While>
void Statement() {
	PrintRule(20);
	// Check if <Compound>
	if (token.value == "{" || token.value == "}") {
		Compound();
	}


	// Check if <Assign>
	else if (token.type == "identifier") {
		Assign();
	}

	// Check <If>
	else if (token.value == "if") {
		If();
	}

	// Check <Return>
	else if (token.value == "return") {
		Return();
	}

	// Check <Print>
	else if (token.value == "put") {
		Print();
	}

	// Check <Scan>
	else if (token.value == "get") {
		Scan();
	}

	// Check <While>
	else if (token.value == "while") {
		While();
	}

	else Error();
}

//R21: <Compound> → { <Statement List> }
void Compound() {
	PrintRule(21);
	if (token.value == "{" || token.value == "}") {
		NextToken();
		StatementList();
	}
	else Error();
}

//R22: <Assign> → <Identifier> = <Expression> ;
void Assign() {
	Token save; // used to store previous token
	PrintRule(22);
	if (token.type == "identifier") {
		save = token;
		Identifier();
		if (token.value == "=") {
			NextToken();
			Expression();
			int addr; // used to store address of identifier for instruction table
					  // Looping through symbol table vector looking for identifier value
			for (int i = 0; i < symbolTable.size(); i++) {
				if (symbolTable.at(i).name == save.value) {
					addr = symbolTable.at(i).addr;
				}
			}
			string strTemp = to_string(addr);
			gen_instr("POPM", strTemp);
			if (token.value == ";") {
				NextToken();
				PrintToken(true);
			}
			else
				Error();
		}
		else
			Error();
	}
	else
		Error();
}

//use the original rule of <If> because our previous rule removed LR which did not work
//R23: <If> → if ( <Condition> ) <Statement> endif | if ( <Condition> ) <Statement> else <statement> endif
void If() {
	PrintRule(23);
	if (token.value == "if") {
		int addr = instr_address;
        NextToken();
        if (token.value == "(") {
            NextToken();
            Condition();
            if (token.value == ")") {
                NextToken();
                Statement();
				back_patch(instr_address);
                if (token.value == "endif") {
                    NextToken();
                }
                else if (token.value == "else") {
                    NextToken();
                    Statement();
                    if (token.value == "endif") {
                        NextToken();
                    }
                    else {
						Error();
                    }
                }
                else {
					Error();
                }
            }
            else {
				Error();
            }
        }
        else {
			Error();
        }
    }
}

//R25: <Return> → return <Return’> ;
void Return() {
	PrintRule(25);
	if (token.value == "return") {
		NextToken();
		ReturnP();
		if (token.value == ";") {
			NextToken();
		}
		else
			Error();
	}
	else Error();
}

//R26: <Return’> → <Expression> | 𝜀
void ReturnP() {
	PrintRule(26);
	if (token.type == "Not Token" || token.type == "COMMENT") {
		Empty();
	}
	else {
		Expression();
	}

}

//R27: <Print> → put (<Expression>);
void Print() {
	PrintRule(27);
	if (token.value == "put") {
	    gen_instr("STDOUT", "");
		NextToken();
		if (token.value == "(") {
			NextToken();
			Expression();
			if (token.value == ")") {
				NextToken();
				if (token.value == ";") {
					NextToken();
				}
				else Error();
			}
			else Error();
		}
		else Error();
	}
	else Error();
}

//R28: <Scan> → get(<IDs>);
void Scan() {
	PrintRule(28);
	if (token.value == "get") {
	    gen_instr("STDIN","");
		NextToken();
		if (token.value == "(") {
			NextToken();
			IDs();
			if (token.value == ")") {
				NextToken();
				if (token.value == ";") {
					NextToken();
				}
				else Error();
			}
			else Error();
		}
		else Error();
	}
	else Error();
}

//R29: <While> → while (<Condition>) <Statement>
void While() {
	PrintRule(29);
	if (token.value == "while") {
		int addr = instr_address;
		gen_instr("LABEL", "");
		NextToken();
		if (token.value == "(") {
			NextToken();
			Condition();
			if (token.value == ")") {
				NextToken();
				Statement();
				string temp = to_string(addr);
				gen_instr("JUMP", temp);
				back_patch(instr_address);
			}
			else Error();
		}
		else Error();
	}
	else Error();
}

//R30: <Condition> → <Expression> <Relop> <Expression>
void Condition() {
	PrintRule(30);
	Expression();
	Relop();
	Expression();
}

//R31: <Relop> → == | ^= | > | < | => | =<
void Relop() {
	PrintRule(31);
	string op = token.value;

	if (token.value == "==" || token.value == "^=" || token.value == ">" ||
		token.value == "<" || token.value == "=>" || token.value == "=<") {
		if (token.value == "<") {
			gen_instr("LES", "");
			jumpVect.push_back(instr_address);
			gen_instr("JUMPZ", "");
		}
		else if (token.value == ">") {
			gen_instr("GRT", "");
            jumpVect.push_back(instr_address);
            gen_instr("JUMPZ", "");
		}
		else if (token.value == "==") {
			gen_instr("EQU", "");
            jumpVect.push_back(instr_address);
            gen_instr("JUMPZ", "");
		}
		else if (token.value == "^=") {
			gen_instr("NEQ", "");
            jumpVect.push_back(instr_address);
            gen_instr("JUMPZ", "");
		}
		else if (token.value == "=>") {
			gen_instr("GEQ", "");
            jumpVect.push_back(instr_address);
            gen_instr("JUMPZ", "");
		}
		else /*if (token.value == "=<")*/ {
			gen_instr("LEQ", "");
            jumpVect.push_back(instr_address);
            gen_instr("JUMPZ", "");
		}
		NextToken();
	}
	else Error();
}

//R32: <Expression> → <Term> <Expression’>
void Expression() {
	PrintRule(32);
	Term();
	ExpressionP();
}

//R33: <Expression’> → + <Term> <Expression’> | - <Term> <Expression’> | 𝜀
void ExpressionP() {
	PrintRule(33);
	if (token.value == "+") {
		NextToken();
		Term();
		gen_instr("ADD", "");
		ExpressionP();
	}
	else if (token.value == "-") {
		NextToken();
		Term();
		gen_instr("SUB", "");
		ExpressionP();
	}
	else Empty();
}

//R34: <Term> → <Factor> <Term’>
void Term() {
	PrintRule(34);
	Factor();
	TermP();
}

//R35: <Term’> → * <Factor> <Term’> | / <Factor> <Term’> | 𝜀
void TermP() {
	PrintRule(35);
	if (token.value == "*") {
		NextToken();
		Factor();
		gen_instr("MUL", "");
		TermP();
	}
	if (token.value == "/") {
		NextToken();
		Factor();
		gen_instr("DIV", "");
		TermP();
	}
	else Empty();
}

//R36: <Factor> → - <Primary> | <Primary>
void Factor() {
	PrintRule(36);
	if (token.value == "-") {
		NextToken();
		Primary();
	}
	else {
		Primary();
	}
}

//R37: <Primary> → <Identifier> | <Integer> | <Identifier> (<IDs>) | (<Expression>) | <Real> | true | false
void Primary() {
	PrintRule(37);
	Token save = token;
	if (token.type == "identifier") {
		if (checkID().declared) {
			//check type
		}
		else {
			fstream coutfile(outputFile, ios_base::app);
			coutfile << "Error! " << token.value << " is not declared!" << endl;
			coutfile.close();
			exit(0);
		}
		int addr; // used to store address of identifier for instruction table
				  // Looping through symbol table vector looking for identifier value
		int ind = checkID().symbolIndex;
		string temp = to_string(symbolTable[ind].addr);
		gen_instr("PUSHM", temp);
		Identifier();
		if (token.value == "[") {
			NextToken();
			IDs();
			if (token.type == "]") {
				NextToken();
			}
			else
				Error();
		}
		else {

		}
	}
	else if (token.type == "integer") {
		Integer();
	}
	else if (token.value == "(") {
		NextToken();
		Expression();
		if (token.value == ")") {
			NextToken();
		}
		else Error();
	}
	else if (token.type == "real") {
		Real();
	}
	else if (token.value == "true" || token.value == "false") {
		NextToken();
	}
	else Error();
}

//R38: <Empty> → 𝜀
void Empty() {
	PrintRule(38);
}

void PrintRule(int ruleNum) {
	fstream coutfile(outputFile, ios_base::app);
	switch (100) {
	case 1:
		coutfile << "<Rat18S> → %% <Opt Declaration List> <Statement List>" << endl;
		break;
	case 2:
		coutfile << "<Opt Function Definitions> → <Function Definitions> | <Empty>" << endl;
		break;
	case 3:
		coutfile << "<Function Definitions> → <Function> <Function Definitions’>" << endl;
		break;
	case 4:
		coutfile << "<Function Definitions’> → <Function Definitions> |  e" << endl;
		break;
	case 5:
		coutfile << "<Function> → function <Identifier> [<Opt Parameter List>] <Opt Declaration List> <Body>" << endl;
		break;
	case 6:
		coutfile << "<Opt Parameter List> → <Parameter List> | <Empty>" << endl;
		break;
	case 7:
		coutfile << "<Parameter List> → <Parameter> <Parameter List’>" << endl;
		break;
	case 8:
		coutfile << "<Parameter List’> → , <Parameter List> |  e" << endl;
		break;
	case 9:
		coutfile << "<Parameter> → <IDs> : <Qualifier>" << endl;
		break;
	case 10:
		coutfile << "<Qualifier> → int | boolean | real" << endl;
		break;
	case 11:
		coutfile << "<Body> → { <Statement List> }" << endl;
		break;
	case 12:
		coutfile << "<Opt Declaration List> → <Declaration List> | <Empty>" << endl;
		break;
	case 13:
		coutfile << "<Declaration List> → <Declaration> ; <Declaration List’>" << endl;
		break;
	case 14:
		coutfile << "<Declaration List’> → <Declaration List> | e" << endl;
		break;
	case 15:
		coutfile << "<Declaration> → <Qualifier> <IDs>" << endl;
		break;
	case 16:
		coutfile << "<IDs> → <Identifier> <IDs’>" << endl;
		break;
	case 17:
		coutfile << "<IDs’> → , <IDs> | e" << endl;
		break;
	case 18:
		coutfile << "<Statement List> → <Statement> <Statement List’>" << endl;
		break;
	case 19:
		coutfile << "<Statement List’> → <Statement List> | e" << endl;
		break;
	case 20:
		coutfile << "<Statement> → <Compound> | <Assign> | <If> | <Return> | <Print> | <Scan> | <While>" << endl;
		break;
	case 21:
		coutfile << "<Compound> → { <Statement List> }" << endl;
		break;
	case 22:
		coutfile << "<Assign> → <Identifier> = <Expression> ;" << endl;
		break;
	case 23:
		coutfile << "<If> → if ( <Condition> ) <Statement> endif | if ( <Condition> ) <Statement> else <statement> endif" << endl;
		break;
	case 24:
		coutfile << "<If’> → else <Statement> | e" << endl;
		break;
	case 25:
		coutfile << "<Return> → return <Return’> ;" << endl;
		break;
	case 26:
		coutfile << "<Return’> → <Expression> | e" << endl;
		break;
	case 27:
		coutfile << "<Print> → put (<Expression>);" << endl;
		break;
	case 28:
		coutfile << "<Scan> → get(<IDs>);" << endl;
		break;
	case 29:
		coutfile << "<While> → while (<Condition>) <Statement>" << endl;
		break;
	case 30:
		coutfile << "<Condition> → <Expression> <Relop> <Expression>" << endl;
		break;
	case 31:
		coutfile << "<Relop> → == | ^= | > | < | => | =<" << endl;
		break;
	case 32:
		coutfile << "<Expression> → <Term> <Expression’>" << endl;
		break;
	case 33:
		coutfile << "<Expression’> → + <Term> <Expression’> | - <Term> <Expression’> | e" << endl;
		break;
	case 34:
		coutfile << "<Term> → <Factor> <Term’>" << endl;
		break;
	case 35:
		coutfile << "<Term’> → * <Factor> <Term’> | / <Factor> <Term’> | e" << endl;
		break;
	case 36:
		coutfile << "<Factor> → - <Primary> | <Primary>" << endl;
		break;
	case 37:
		coutfile << "<Primary> → <Identifier> | <Integer> | <Identifier> (<IDs>) | (<Expression>) | <Real> | true | false" << endl;
		break;
	case 38:
		coutfile << "<Empty> → e" << endl;
		break;
	default:
		//coutfile << "Syntax Error" << endl;
		break;
	}
	coutfile.close();
}

void Identifier() {
	if (token.type == "identifier") {
//	    int i = checkID().symbolIndex;
//	    int addr = symbolTable[i].addr;
//	    string temp = to_string(addr);
//	    gen_instr("PUSHM", temp );
		NextToken();
	}
	else Error();
}

void Real() {
	if (token.type == "real") {
		NextToken();
	}
	else Error();
}

void Integer() {
	if (token.type == "integer") {
	    gen_instr("PUSHI", token.value);
		NextToken();
	}
	else Error();
}

void gen_instr(string op, string oprnd) {
	Instr temp = Instr();
	temp.addr = instr_address;
	temp.optor = op;
	temp.oprand = oprnd;
	instrTable.push_back(temp);
	instr_address++;
}

void back_patch(int jump_addr) {
	int addr = jumpVect.back(); // used for index for back patching
	//jumpStack.pop();
	jumpVect.pop_back();
	string temp = to_string(jump_addr);
	instrTable[addr-1].oprand = temp;
}
