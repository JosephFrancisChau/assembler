//#include "stdafx.h"
#include "parser.h"
#include "lexer.h"
#include <fstream>
#include "globals.h"
#include <iomanip>  //setw

//testing

using namespace std;
//fstream coutfile(outputFile, ios_base::app);
//vector<string> allWords;
Token token;
int ruleNum;
//string s;
//unsigned tokenIndex;

//fstream coutfile(outputFile, std::ios_base::app);
//ofstream coutfile;

void Parser(Token word, unsigned tokenIndex) {
    token = word;
    Rat18S(token, tokenIndex);
}

void Error() {
    fstream coutfile(outputFile, ios_base::app);
    cout << "Syntax Error!" << endl;
    coutfile << "Syntax Error!" << endl;
    coutfile.close();
    exit(0);
}

//R1: <Rat18S> → <Opt Function Definitions> %% <Opt Declaration List> <Statement List>
void Rat18S(Token token, unsigned tokenIndex) {

    PrintRule(1);
    OptFunctionDefinitions();

    fstream coutfile(outputFile, ios_base::app);
    Token temp = lexer(allWords.at(++tokenIndex));
    cout << left << setw(10) << "Token:" << temp.type << "\t\t" << temp.value << endl;
    coutfile << left << setw(10) << temp.type << "\t\t" << temp.value << endl;
    coutfile.close();

    if (temp.value == "%%") {
        Token temp1 = lexer(allWords.at(++tokenIndex));
        OptDeclarationList(temp1);
        Token temp2 = lexer(allWords.at(++tokenIndex));
        StatementList(temp2);
    }
    else Error();
    if (token.value != "$") Error();
}

//R2: <Opt Function Definitions> → <Function Definitions> | <Empty>
void OptFunctionDefinitions() {
    PrintRule(2);
    if (token.type == "NotToken" || token.type == "COMMENT") {
        Empty();
    }
    else {
        FunctionDefinitions();
    }
}

//R3: <Function Definitions> → <Function> <Function Definitions’>
void FunctionDefinitions() {
    fstream coutfile(outputFile, ios_base::app);
    PrintRule(3);
    Function();
    Token temp1 = lexer(allWords.at(++tokenIndex));
    coutfile << left << setw(10) << temp1.type << "\t\t" << temp1.value << endl;
    FunctionDefinitionsP(temp1);
}

//R4: <Function Definitions’> → <Function Definitions> |  𝜀
void FunctionDefinitionsP(Token t) {
    PrintRule(4);
    if (t.type == "NotToken" || t.type == "COMMENT") {
        Empty();
    }
    else {
        FunctionDefinitions();
    }
}

//R5: <Function> → function <Identifier> [<Opt Parameter List>] <Opt Declaration List> <Body>
void Function() {
    PrintRule(5);
    if (token.value == "function") {
        Token temp1 = lexer(allWords.at(++tokenIndex));
        fstream coutfile(outputFile, ios_base::app);
        coutfile << left << setw(10) << temp1.type << "\t\t" << temp1.value << endl;
        Identifier(temp1);
        Token temp2 = lexer(allWords.at(++tokenIndex));
        coutfile << left << setw(10) << temp2.type << "\t\t" << temp2.value << endl;
        if (temp2.value == "[") {
            Token temp3 = lexer(allWords.at(++tokenIndex));
            coutfile << left << setw(10) << temp3.type << "\t\t" << temp3.value << endl;
            OptParameterList(temp3);
            Token temp4 = lexer(allWords.at(++tokenIndex));
            coutfile << left << setw(10) << temp4.type << "\t\t" << temp4.value << endl;
            if (temp4.value == "]") {
                Token temp5 = lexer(allWords.at(++tokenIndex));
                coutfile << left << setw(10) << temp5.type << "\t\t" << temp5.value << endl;
                OptDeclarationList(temp5);
                Token temp6 = lexer(allWords.at(++tokenIndex));
                coutfile << left << setw(10) << temp6.type << "\t\t" << temp6.value << endl;
                Body(temp6);
            }
            else Error();
        }
        else Error();
    }
    else Error();
}

//R6: <Opt Parameter List> → <Parameter List> | <Empty>
void OptParameterList(Token t) {
    PrintRule(6);
    if (t.type == "NotToken" || t.type == "COMMENT") {
        Empty();
    }
    else {
        ParameterList(t);
    }
}

//R7: <Parameter List> → <Parameter> <Parameter List’>
void ParameterList(Token t) {
    PrintRule(7);
    Parameter(t);
    Token temp1 = lexer(allWords.at(++tokenIndex));
    fstream coutfile(outputFile, ios_base::app);
    coutfile << left << setw(10) << temp1.type << "\t\t" << temp1.value << endl;
    ParameterListP(temp1);
}

//R8: <Parameter List’> → , <Parameter List> |  𝜀
void ParameterListP(Token t) {
    PrintRule(8);
    if (t.type == "NotToken" || t.type == "COMMENT" || t.value != ",") {
        Empty();
    }
    else if (t.value == ",") {
        Token temp1 = lexer(allWords.at(++tokenIndex));
        fstream coutfile(outputFile, ios_base::app);
        coutfile << left << setw(10) << temp1.type << "\t\t" << temp1.value << endl;
        ParameterList(temp1);
    }
    else Error();
}

//R9: <Parameter> → <IDs> : <Qualifier>
void Parameter(Token t) {
    PrintRule(9);
    IDs(t);
    Token temp = lexer(allWords.at(++tokenIndex));
    fstream coutfile(outputFile, ios_base::app);
    coutfile << left << setw(10) << temp.type << "\t\t" << temp.value << endl;
    if (temp.value == ":") {
        Token temp1 = lexer(allWords.at(++tokenIndex));
        coutfile << left << setw(10) << temp1.type << "\t\t" << temp1.value << endl;
        Qualifier(temp1);
    }
    else Error();
}

//R10: <Qualifier> → int | boolean | real
void Qualifier(Token t) {
    PrintRule(10);
    if (t.value == "int" || t.value == "boolean" || t.value == "real") {}
    else Error();
}

//R11: <Body> → { <Statement List> }
void Body(Token t) {
    PrintRule(11);
    if (t.value == "{") {
        Token temp1 = lexer(allWords.at(++tokenIndex));
        fstream coutfile(outputFile, ios_base::app);
        coutfile << left << setw(10) << temp1.type << "\t\t" << temp1.value << endl;
        StatementList(temp1);
        Token temp = lexer(allWords.at(++tokenIndex));
        coutfile << left << setw(10) << temp.type << "\t\t" << temp.value << endl;
        if (temp.value == "}") {}
        else Error();
    }
    //else Error();
}

//R12: <Opt Declaration List> → <Declaration List> | <Empty>
void OptDeclarationList(Token t) {
    PrintRule(12);
    if (t.type == "NotToken" || t.type == "COMMENT") {
        Empty();
    }
    else {
        DeclarationList();
    }
}

//R13: <Declaration List> → <Declaration> ; <Declaration List’>
void DeclarationList() {
    PrintRule(13);
    Declaration();
    Token temp = lexer(allWords.at(++tokenIndex));
    fstream coutfile(outputFile, ios_base::app);
    coutfile << left << setw(10) << temp.type << "\t\t" << temp.value << endl;
    if (temp.value == ";") {
        Token temp1 = lexer(allWords.at(++tokenIndex));
        coutfile << left << setw(10) << temp1.type << "\t\t" << temp1.value << endl;
        DeclarationListP(temp1);
    }
    else
        Error();
}

//R14: <Declaration List’> → <Declaration List> | <Empty>
void DeclarationListP(Token t) {
    PrintRule(14);
    if (t.type == "NotToken" || t.type == "COMMENT") {
        Empty();
    }
    else {
        DeclarationList();
    }
}

//R15: <Declaration> → <Qualifier> <IDs>
void Declaration() {
    PrintRule(15);
    Qualifier(token);
    Token temp = lexer(allWords.at(++tokenIndex));
    fstream coutfile(outputFile, ios_base::app);
    coutfile << left << setw(10) << temp.type << "\t\t" << temp.value << endl;
    IDs(temp);
}

//R16: <IDs> → <Identifier> <IDs’>
void IDs(Token t) {
    PrintRule(16);
    Identifier(t);
    Token temp = lexer(allWords.at(++tokenIndex));
    fstream coutfile(outputFile, ios_base::app);
    coutfile << left << setw(10) << temp.type << "\t\t" << temp.value << endl;
    IDsP(temp);
}

//R17: <IDs’> → , <IDs> | <Empty>
void IDsP(Token t) {
    PrintRule(17);
    if (t.type == "NotToken" || t.type == "COMMENT" || t.value != ",") {
        Empty();
    }
    if (t.value == ",") {
        Token temp1 = lexer(allWords.at(++tokenIndex));
        fstream coutfile(outputFile, ios_base::app);
        coutfile << left << setw(10) << temp1.type << "\t\t" << temp1.value << endl;
        IDs(temp1);
    }
    //else    Error();
}

//R18: <Statement List> → <Statement> <Statement List’>
void StatementList(Token t) {
    PrintRule(18);
    Statement(t);
    Token temp1 = lexer(allWords.at(++tokenIndex));
    fstream coutfile(outputFile, ios_base::app);
    coutfile << left << setw(10) << temp1.type << "\t\t" << temp1.value << endl;
    StatementListP(temp1);
}

//R19: <Statement List’> → <Statement List> | <Empty>
void StatementListP(Token t) {
    PrintRule(19);
    if (t.type == "NotToken" || t.type == "COMMENT") {
        Empty();
    }
    else {
        StatementList(t);
    }
}

//R20: <Statement> → <Compound> | <Assign> | <If> | <Return> | <Print> | <Scan> | <While>
void Statement(Token t) {
    PrintRule(20);
    Token temp = lexer(allWords.at(++tokenIndex));
    fstream coutfile(outputFile, ios_base::app);
    coutfile << left << setw(10) << temp.type << "\t\t" << temp.value << endl;

    // Check if <Compound>
    if (t.value == "{") {
        Compound();
    }

    // Check if <Assign>
    else if (temp.type == "identifier") {
        Assign();
    }

    // Check <If>
    else if (t.type == "keyword" && t.value == "if") {
        If();
    }

    // Check <Return>
    else if (t.type == "keyword" && t.value == "return") {
        Return();
    }

    // Check <Print>
    else if (t.type == "keyword" && t.value == "put") {
        Print();
    }

    // Check <Scan>
    else if (t.type == "keyword" && t.value == "get") {
        Scan();
    }

    // Check <While>
    else if (t.type == "keyword" && t.value == "while") {
        While();
    }

    else Error();
}

//R21: <Compound> → { <Statement List> }
void Compound() {
    PrintRule(21);
    if (token.value == "{") {
        Token temp1 = lexer(allWords.at(++tokenIndex));
        fstream coutfile(outputFile, ios_base::app);
        coutfile << left << setw(10) << temp1.type << "\t\t" << temp1.value << endl;
        StatementList(temp1);
        Token temp2 = lexer(allWords.at(++tokenIndex));
        coutfile << left << setw(10) << temp2.type << "\t\t" << temp2.value << endl;
        if (temp2.value == "}") {}
        else Error();
    }
    else Error();
}

//R22: <Assign> → <Identifier> = <Expressions> ;
void Assign() {
    PrintRule(22);
    Identifier(token);
    Token temp = lexer(allWords.at(++tokenIndex));
    fstream coutfile(outputFile, ios_base::app);
    coutfile << left << setw(10) << temp.type << "\t\t" << temp.value << endl;
    if (temp.value == "=") {
        Token temp1 = lexer(allWords.at(++tokenIndex));
        coutfile << left << setw(10) << temp1.type << "\t\t" << temp1.value << endl;
        Expression(temp1);
    }
    else
        Error();
}

//R23: <If> → if ( <Condition> ) <Statement> <If’> endif
void If() {
    PrintRule(23);
    if (token.value == "if") {
        Token temp1 = lexer(allWords.at(++tokenIndex));
        fstream coutfile(outputFile, ios_base::app);
        coutfile << left << setw(10) << temp1.type << "\t\t" << temp1.value << endl;
        if (temp1.value == "(") {
            Token temp2 = lexer(allWords.at(++tokenIndex));
            coutfile << left << setw(10) << temp2.type << "\t\t" << temp2.value << endl;
            Condition(temp2);
            Token temp3 = lexer(allWords.at(++tokenIndex));
            coutfile << left << setw(10) << temp3.type << "\t\t" << temp3.value << endl;
            if (temp3.value == ")") {
                Token temp4 = lexer(allWords.at(++tokenIndex));
                coutfile << left << setw(10) << temp4.type << "\t\t" << temp4.value << endl;
                Statement(temp4);
                Token temp5 = lexer(allWords.at(++tokenIndex));
                coutfile << left << setw(10) << temp5.type << "\t\t" << temp5.value << endl;
                IfP(temp5);
                Token temp6 = lexer(allWords.at(++tokenIndex));
                coutfile << left << setw(10) << temp6.type << "\t\t" << temp6.value << endl;
                if (temp6.value == "endif") {}
                else
                    Error();
            }
        }
    }
    else Error();
}

//R24: <If’> → else <Statement> | <Empty>
void IfP(Token t) {
    PrintRule(24);
    if (t.type == "NotToken" || t.type == "COMMENT") {
        Empty();
    }
    else {
        if (t.value == "else") {
            Token temp1 = lexer(allWords.at(++tokenIndex));
            fstream coutfile(outputFile, ios_base::app);
            coutfile << left << setw(10) << temp1.type << "\t\t" << temp1.value << endl;
            Statement(temp1);
        }
        else
            Error();
    }
}

//R25: <Return> → return <Return’> ;
void Return() {
    PrintRule(25);
    if (token.value == "return") {
        Token temp1 = lexer(allWords.at(++tokenIndex));
        fstream coutfile(outputFile, ios_base::app);
        coutfile << left << setw(10) << temp1.type << "\t\t" << temp1.value << endl;
        ReturnP(temp1);
        Token temp2 = lexer(allWords.at(++tokenIndex));
        coutfile << left << setw(10) << temp2.type << "\t\t" << temp2.value << endl;
        if (temp2.value == ";") {}
        else
            Error();
    }
    else Error();
}

//R26: <Return’> → <Expression> | 𝜀
void ReturnP(Token t) {
    PrintRule(26);
    if (t.type == "Not Token" || t.type == "COMMENT") {
        Empty();
    }
    else {
        Expression(t);
    }

}

//R27: <Print> → put (<Expression>);
void Print() {
    PrintRule(27);
    if (token.value == "put") {
        Token temp1 = lexer(allWords.at(++tokenIndex));
        fstream coutfile(outputFile, ios_base::app);
        coutfile << left << setw(10) << temp1.type << "\t\t" << temp1.value << endl;
        if (temp1.value == "(") {
            Token temp2 = lexer(allWords.at(++tokenIndex));
            coutfile << left << setw(10) << temp2.type << "\t\t" << temp2.value << endl;
            Expression(temp2);
            Token temp3 = lexer(allWords.at(++tokenIndex));
            coutfile << left << setw(10) << temp3.type << "\t\t" << temp3.value << endl;
            if (temp3.value == ")") {
                Token temp4 = lexer(allWords.at(++tokenIndex));
                coutfile << left << setw(10) << temp4.type << "\t\t" << temp4.value << endl;
                if (temp4.value == ";") {}
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
        Token temp1 = lexer(allWords.at(++tokenIndex));
        fstream coutfile(outputFile, ios_base::app);
        coutfile << left << setw(10) << temp1.type << "\t\t" << temp1.value << endl;
        if (temp1.value == "(") {
            Token temp2 = lexer(allWords.at(++tokenIndex));
            coutfile << left << setw(10) << temp2.type << "\t\t" << temp2.value << endl;
            IDs(temp2);
            Token temp3 = lexer(allWords.at(++tokenIndex));
            coutfile << left << setw(10) << temp3.type << "\t\t" << temp3.value << endl;
            if (temp3.value == ")") {
                Token temp4 = lexer(allWords.at(++tokenIndex));
                coutfile << left << setw(10) << temp4.type << "\t\t" << temp4.value << endl;
                if (temp4.value == ";") {}
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
        Token temp1 = lexer(allWords.at(++tokenIndex));
        fstream coutfile(outputFile, ios_base::app);
        coutfile << left << setw(10) << temp1.type << "\t\t" << temp1.value << endl;
        if (temp1.value == "(") {
            Token temp2 = lexer(allWords.at(++tokenIndex));
            coutfile << left << setw(10) << temp2.type << "\t\t" << temp2.value << endl;
            Condition(temp2);
            Token temp3 = lexer(allWords.at(++tokenIndex));
            coutfile << left << setw(10) << temp3.type << "\t\t" << temp3.value << endl;
            if (temp3.value == ")") {
                Token temp4 = lexer(allWords.at(++tokenIndex));
                coutfile << left << setw(10) << temp4.type << "\t\t" << temp4.value << endl;
                Statement(temp4);
            }
            else Error();
        }
        else Error();
    }
    else Error();
}

//R30: <Condition> → <Expression> <Relop> <Expression>
void Condition(Token t) {
    PrintRule(30);
    Expression(t);
    Token temp1 = lexer(allWords.at(++tokenIndex));
    fstream coutfile(outputFile, ios_base::app);
    coutfile << left << setw(10) << temp1.type << "\t\t" << temp1.value << endl;
    Relop(temp1);
    Token temp2 = lexer(allWords.at(++tokenIndex));
    coutfile << left << setw(10) << temp2.type << "\t\t" << temp2.value << endl;
    Expression(temp2);
}

//R31: <Relop> → == | ^= | > | < | => | =<
void Relop(Token t) {
    PrintRule(31);
    if (t.value == "==" || t.value == "^=" || t.value == ">" ||
        t.value == "<" || t.value == "=>" || t.value == "=<") {
    }
    else Error();
}

//R32: <Expression> → <Term> <Expression’>
void Expression(Token t) {
    PrintRule(32);
    Term(t);
    Token temp1 = lexer(allWords.at(++tokenIndex));
    fstream coutfile(outputFile, ios_base::app);
    coutfile << left << setw(10) << temp1.type << "\t\t" << temp1.value << endl;
    ExpressionP(temp1);
}

//R33: <Expression’> → + <Term> <Expression’> | - <Term> <Expression’> | 𝜀
void ExpressionP(Token t) {
    PrintRule(33);
    if (t.type == "Not Token" || t.type == "COMMENT") {
        Empty();
    }
    else if (t.value == "+" || t.value == "-") {
        Token temp1 = lexer(allWords.at(++tokenIndex));
        fstream coutfile(outputFile, ios_base::app);
        coutfile << left << setw(10) << temp1.type << "\t\t" << temp1.value << endl;
        Term(temp1);
        Token temp2 = lexer(allWords.at(++tokenIndex));
        coutfile << left << setw(10) << temp2.type << "\t\t" << temp2.value << endl;
        ExpressionP(temp2);
    }
    else Error();
}

//R34: <Term> → <Factor> <Term’>
void Term(Token t) {
    PrintRule(34);
    Factor(t);
    Token temp1 = lexer(allWords.at(++tokenIndex));
    fstream coutfile(outputFile, ios_base::app);
    coutfile << left << setw(10) << temp1.type << "\t\t" << temp1.value << endl;
    TermP(temp1);
}

//R35: <Term’> → * <Factor> <Term’> | / <Factor> <Term’> | 𝜀
void TermP(Token t) {
    PrintRule(35);
    if (token.type == "Not Token" || token.type == "COMMENT") {
        Empty();
    }
    else if (token.value == "*" || token.value == "/") {
        Token temp1 = lexer(allWords.at(++tokenIndex));
        fstream coutfile(outputFile, ios_base::app);
        coutfile << left << setw(10) << temp1.type << "\t\t" << temp1.value << endl;
        Factor(temp1);
        Token temp2 = lexer(allWords.at(++tokenIndex));
        coutfile << left << setw(10) << temp2.type << "\t\t" << temp2.value << endl;
        TermP(temp2);
    }
    else Error();
}

//R36: <Factor> → - <Primary> | <Primary>
void Factor(Token t) {
    PrintRule(36);
    Token temp = lexer(allWords.at(++tokenIndex));
    fstream coutfile(outputFile, ios_base::app);
    coutfile << left << setw(10) << temp.type << "\t\t" << temp.value << endl;
    if (t.value == "-") {
        Primary(temp);
    }
    else {
        Primary(token);
    }
}

//R37: <Primary> → <Identifier> | <Integer> | <Identifier> (<IDs>) | (<Expression>) | <Real> | true | false
void Primary(Token t) {
    PrintRule(37);
    if (t.type == "identifier") {
        Identifier(token);
        Token temp1 = lexer(allWords.at(++tokenIndex));
        fstream coutfile(outputFile, ios_base::app);
        coutfile << left << setw(10) << temp1.type << "\t\t" << temp1.value << endl;
        if (temp1.value == "(") {
            Token temp2 = lexer(allWords.at(++tokenIndex));
            coutfile << left << setw(10) << temp2.type << "\t\t" << temp2.value << endl;
            IDs(temp2);
            Token temp3 = lexer(allWords.at(++tokenIndex));
            coutfile << left << setw(10) << temp3.type << "\t\t" << temp3.value << endl;
            if (temp3.value == ")") {}
            else Error();
        }
    }
    else if (t.type == "integer") {
        Integer();
    }
    else if (t.value == "(") {
        Token temp1 = lexer(allWords.at(++tokenIndex));
        fstream coutfile(outputFile, ios_base::app);
        coutfile << left << setw(10) << temp1.type << "\t\t" << temp1.value << endl;
        Expression(temp1);
        Token temp2 = lexer(allWords.at(++tokenIndex));
        coutfile << left << setw(10) << temp2.type << "\t\t" << temp2.value << endl;
        if (temp2.value == ")") {}
        else Error();
    }
    else if (t.type == "real") {
        Real();
    }
    else if (t.value == "true" || t.value == "false") {}
    else Error();
}

//R38: <Empty> → 𝜀
void Empty() {
    PrintRule(38);
    tokenIndex--;
}

void PrintRule(int ruleNum) {
    fstream coutfile(outputFile, ios_base::app);
    switch (ruleNum) {
    case 1:
        coutfile << "<Rat18S> → <Opt Function Definitions> %% <Opt Declaration List> <Statement List>" << endl;
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
        coutfile << "<Assign> → <Identifier> = <Expressions> ;" << endl;
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

void Identifier(Token temp) {
    if (temp.type == "identifier") {
    }
    //else
    //Error();
}

void Real() {
    if (token.type == "real") {
    }
    else
        Error();
}

void Integer() {
    if (token.type == "integer") {
    }
    else
        Error();
}
