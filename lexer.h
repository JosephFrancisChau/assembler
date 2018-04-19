#pragma once
#include <string>
#include <vector>
using namespace std;

struct Token {
	string type;
	string value;
};

vector<string> splitWord(string& word);
Token lexer(string& word); 
