#pragma once
#include <fstream>
#include <iostream>
#include "lexer.h"
#include "globals.h"

void Parser(vector<string> vec);
void Error();
void PrintToken(bool doit);
void NextToken();
void PrintRule(int ruleNum);

struct Symbol {
	int addr;
	string name;
	string type;
};
struct Instr {
	int addr;
	int oprand;
	string optor;

};
struct Pair {
	bool declared;
	unsigned symbolIndex;
};

Pair checkID();
void printSymbol();
void printInstr();
//R1: <Rat18S> → <Opt Function Definitions> %% <Opt Declaration List> <Statement List>
void Rat18S();

//R6: <Opt Parameter List> → <Parameter List> | <Empty>
void OptParameterList();

//R7: <Parameter List> → <Parameter> <Parameter List’>
void ParameterList();

//R8: <Parameter List’> → , <Parameter List> |  𝜀
void ParameterListP();

//R9: <Parameter> → <IDs> : <Qualifier>
void Parameter();

//R10: <Qualifier> → int | boolean | real
void Qualifier();

//R11:    <Body> → { <Statement List> }
void Body();

//R12: <Opt Declaration List> → <Declaration List> | <Empty>
void OptDeclarationList();

//R13:  <Declaration List> → <Declaration> ; <Declaration List’>
void DeclarationList();

//R14: <Declaration List’> → <Declaration List> | <Empty>
void DeclarationListP();

//R15: <Declaration> → <Qualifier> <IDs>
void Declaration();

//R16: <IDs> → <Identifier> <IDs’>
void IDs();

//R17: <IDs’> → , <IDs> | <Empty>
void IDsP();

//R18: <Statement List> → <Statement> <Statement List’>
void StatementList();

//R19: <Statement List’> → <Statement List> | <Empty>
void StatementListP();

//R20: <Statement> → <Compound> | <Assign> | <If> | <Return> | <Print> | <Scan> | <While>
void Statement();

//R21: <Compound> → { <Statement List> }
void Compound();

//R22: <Assign> → <Identifier> = <Expressions> ;
void Assign();

//R23: <If> → if ( <Condition> ) <Statement> <If’> endif
void If();

//R24: <If’> → else <Statement> | <Empty>
void IfP();

//R25: <Return> → return <Return’> ;
void Return();

//R26: <Return’> → <Expression> | 𝜀
void ReturnP();

//R27: <Print> → put (<Expression>);
void Print();

//R28: <Scan> → get(<IDs>);
void Scan();

//R29: <While> → while (<Condition>) <Statement>
void While();

//R30: <Condition> → <Expression> <Relop> <Expression>
void Condition();

//R31: <Relop> → == | ^= | > | < | => | =<
void Relop();

//R32: <Expression> → <Term> <Expression’>
void Expression();

//R33: <Expression’> → + <Term> <Expression’> | - <Term> <Expression’> | 𝜀
void ExpressionP();

//R34: <Term> → <Factor> <Term’>
void Term();

//R35: <Term’> → * <Factor> <Term’> | / <Factor> <Term’> | 𝜀
void TermP();

//R36: <Factor> → - <Primary> | <Primary>
void Factor();

//R37: <Primary> → <Identifier> | <Integer> | <Identifier> (<IDs>) | (<Expression>) | <Real> | true | false
void Primary();

//R38: <Empty> → 𝜀
void Empty();

void Identifier();
void Integer();
void Real();

void gen_instr(string op, int oprnd);

void back_patch(int jump_addr);

//#endif /* parser_h */
