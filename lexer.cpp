//	step 1)call splitWord
//			input: a word
//          output: a vector(name: cleanwords) which stores all the splited words
//	step 2)call lexer
//          input: a splited word
//          output: a token ( type, value )
//			Note (1) : If the word is not a legal token, the returned type is "NotToken".
//          Note (2) : if the word is a comment, the returned type is "COMMENT"

//#include "stdafx.h"
#include "lexer.h"
#include <iostream>
#include <map>
#include <set>
#include <fstream>
#include <sstream>
#include <stack>
#include <ctype.h>

using namespace std;

set<string> operators = { "=","==","^=",">","<","=>","<=","+","-","*","/" };
set<string> separators = { "%%", "[", "]", ";",",", "{","}","(",")","!",":" };
set<string> keywords = { "function", "int", "boolean","real", "if", "endif", "else", "return", "put", "get", "while", "true", "false" };
set<string> delimiters = { "=","==","^=",">","<","=>","<=","+","-","*","/" ,"%%", "[", "]", ";",",", "{","}","(",")","!",":" };
stack<string> comments;
map<size_t, size_t> positions;	//store all the separators and operators address and length
vector<string> cleanwords;

vector<string> splitWord(string& word);
bool testDelimiter(string& word);
Token lexer(string& word);

Token lexer(string& word) {
	Token mytoken;
	mytoken.value = word;
	mytoken.type = "NotToken";
	string temp1, temp2, value;
	size_t len = word.length();
	set<string>::iterator it;
	//check "!"
	if (word == "!") {
		mytoken.type = "COMMENT";
		if (comments.empty())
			comments.push(word);
		else {
			while (!comments.empty()) {
				comments.pop();
			}
		}
		return mytoken;
	}


	else {
		//check comment
		if (!comments.empty()) {
			comments.push(word);
			mytoken.type = "COMMENT";
			return mytoken;
		}
		//if first char is letter, then check keyword and identifier
		if (isalpha(word[0])) {
			bool isKeyword = false, isID = false;
			//check keyword first
			it = keywords.find(word);
			if (it != keywords.end()) {
				//isToken = true;
				isKeyword = true;
				mytoken.type = "keyword";
			}
			//check identifier
			else {
				if (len == 1) {
					//isToken = true;
					isID = true;
					mytoken.type = "identifier";
				}
				else {
					isID = true;
					for (size_t i = 1; i < len - 1; ++i) {
						isID = isID && (isalpha(word[i]) || isdigit(word[i]));
					}
					isID = isID && ((word[len - 1] == '$') || isalpha(word[len - 1]));
					if (isID) {
						mytoken.type = "identifier";
					}
				}
			}
		}

		//if first char is number, then check integer and real
		else if (isdigit(word[0])) {
			//check integer
			bool isInteger = true;
			for (size_t i = 1; i < len; ++i) {
				isInteger = isInteger && isdigit(word[i]);;
			}
			if (isInteger) {
				//isToken = true;
				mytoken.type = "integer";
			}

			//check real;
			bool isReal = true;
			if (len >= 3) {
				if (word.find('.') != string::npos) {
					size_t p = word.find('.');
					string subword1 = word.substr(0, p);
					string subword2 = word.substr(p + 1);
					bool isReal1 = true;
					bool isReal2 = true;
					for (size_t i = 1; i < subword1.length(); ++i) {
						isReal1 = isReal && isdigit(word[i]);
					}
					for (size_t i = 0; i < subword1.length(); ++i) {
						isReal2 = isReal && isdigit(word[i]);
					}
					isReal = isReal1 && isReal2;
					if (isReal) {
						//isToken = true;
						mytoken.type = "real";
					}
				}
			}
		}

		//if the first char is a symbol, then check separator and operator
		else {
			bool isOperator = false;
			bool isSeparator = false;
			//check is operator
			it = operators.find(word);
			if (it != operators.end()) {
				isOperator = true;
				//isToken = true;
				mytoken.type = "operator";
			}

			//check is separator
			it = separators.find(word);
			if (it != separators.end()) {
				isSeparator = true;
				//isToken = true;
				mytoken.type = "separator";
			}
		}

		return mytoken;
	}
}

//input: a word
//output: true, if there is a dilimiter
//output: false, if there is not a dilimiter
bool testDelimiter(string& word) {
	positions.clear();
	bool exist = false;
	size_t i;
	set<string>::iterator it;
	for (it = delimiters.begin(); it != delimiters.end(); ++it) {
		if (word.find(*it) != string::npos) {
			exist = true;
			i = word.find(*it);
			positions.insert(pair<size_t, size_t>(i, (*it).length()));
		}
	}
	if (!exist) cleanwords.push_back(word);
	return exist;
}

vector<string> splitWord(string& word) {
	map<size_t, size_t>::iterator p;
	string sub1, sub2, sub3;
	size_t len, firstD;
	cleanwords.clear();

	while (testDelimiter(word) && !word.empty()) {
		p = positions.begin();
		firstD = p->first;
		len = p->second;

		//split word to two parts, because once there is a sep or ope at least can be split to two parts
		if (firstD == 0) {
			sub1 = word.substr(0, len);
			sub2 = word.substr(len);
			cleanwords.push_back(sub1);	

			//allWords.push_back(sub1);	//to creat allWords
			word = sub2;
		}
		else if (firstD > 0 && firstD < word.length() - 1) {
			sub1 = word.substr(0, firstD);
			sub2 = word.substr(firstD, len);
			sub3 = word.substr(firstD + len);
			cleanwords.push_back(sub1);	
			cleanwords.push_back(sub2);

			//allWords.push_back(sub1);	//to creat allWords
			//allWords.push_back(sub2);	//to creat allWords
			word = sub3;
		}
		else {
			sub1 = word.substr(0, firstD);
			sub2 = word.substr(firstD);

			cleanwords.push_back(sub1);
			cleanwords.push_back(sub2);

			//allWords.push_back(sub1);	//to creat allWords
			//allWords.push_back(sub2);	//to creat allWords
			word.clear();
		}
	}

	return cleanwords;
}
