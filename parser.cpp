#include "stdafx.h"
#include "parser.h"
#include "lexer.h"
#include <fstream>
#include "globals.h"

using namespace std;

vector<string> allWords;
unsigned tokenIndex;
Token token;
int ruleNum;

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
	if (doit) {
		fstream coutfile(outputFile, ios_base::app);
		coutfile << endl << token.type << "\t\t" << token.value << endl;
		doit = false;
		coutfile.close();
	}
}
void NextToken() {
	if (tokenIndex < allWords.size() ){
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
	IDs();
}

//R16: <IDs> → <Identifier> <IDs’>
void IDs() {
	PrintRule(16);
	NextToken();
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
	if (token.value == "{") {
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

//R22: <Assign> → <Identifier> = <Expression> ;
void Assign() {
	PrintRule(22);
	if (token.type == "identifier") {
		NextToken();
		if (token.value == "=") {
			NextToken();
			Expression();
			if (token.value == ";") {
				token = lexer(allWords.at(++tokenIndex));
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

//R23: <If> → if ( <Condition> ) <Statement> <If’> endif
void If() {
	PrintRule(23);
	if (token.value == "if") {
		NextToken();
		if (token.value == "(") {
			NextToken();
			Condition();
			if (token.value == ")") {
				NextToken();
				Statement();
				IfP();
				NextToken();
				if (token.value == "endif") {
					NextToken();
				}
				else
					Error();
			}
		}
	}
	else Error();
}

//R24: <If’> → else <Statement> | <Empty>
void IfP() {
	PrintRule(24);
	if (token.type == "NotToken" || token.type == "COMMENT") {
		Empty();
	}
	else {
		if (token.value == "else") {
			NextToken();
			Statement();
		}
		else
			Error();
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
		NextToken();
		if (token.value == "(") {
			NextToken();
			Expression();
			if (token.value == ")") {
				NextToken();
				if (token.type == ";") {
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
		NextToken();
		if (token.value == "(") {
			NextToken();
			Condition();
			if (token.value == ")") {
				NextToken();
				Statement();
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
	if (token.value == "==" || token.value == "^=" || token.value == ">" ||
		token.value == "<" || token.value == "=>" || token.value == "=<") {
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
	if (token.value == "+" || token.value == "-") {
		NextToken();
		Term();
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
	if  (token.value == "*" || token.value == "/") {
		NextToken();
		Factor();
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
	if (token.type == "identifier") {
		NextToken();
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
	switch (ruleNum) {
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
		coutfile << "<If> →  if ( <Condition> ) <Statement> <If’> endif" << endl;
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
		coutfile << "Syntax Error" << endl;
		break;
	}
	coutfile.close();
}

void Real() {
	if (token.type == "real") {
		NextToken();
	}
	else Error();
}

void Integer() {
	if (token.type == "integer") {
		NextToken();
	}
	else Error();
}
