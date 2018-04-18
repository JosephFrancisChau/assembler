//#include "stdafx.h"
#include "lexer.h"
#include "globals.h"
#include "parser.h"
#include <iomanip>  //setw

using namespace std;
vector<string> allWords;
string outputFile;
Token start;
unsigned tokenIndex;
//fstream coutfile(outputFile);

int main(int argc, const char * argv[]) {
	string inputFile, word, ofile;
	//ifstream sourceFile;


	//test sample source code
	cout << "Enter test file name(format: filename.txt): ";
	cin >> inputFile;

	cout << "Enter test output file name(format: filename.txt): ";
	cin >> outputFile;

	fstream coutfile;
	coutfile.open(outputFile, fstream::out);

	ifstream sourceFile(inputFile);

	if (sourceFile.is_open()) {
		// call splitWord can create allWords
		while (sourceFile >> word) {
			vector<string> vec = splitWord(word);
			for (unsigned i = 0; i < vec.size(); ++i) {
				//allWords.push_back(vec.at(i));
                if(!vec.at(i).empty())
                    allWords.push_back(vec.at(i));
			}
		}
		allWords.push_back("$");	//push "$" after reading in all the words
		
		//test the new code on allWords works or nor
		for (unsigned i = 0; i < allWords.size(); ++i) {
			Token start = lexer(allWords.at(i));
			cout << left << setw(10) << "Token:" << start.type << "\t\t" << "Lexeme: "<< start.value << endl;
			coutfile << left << setw(10) << "Token:" << start.type << "\t\t" << "Lexeme: " << start.value << endl;
			tokenIndex = i;
			Parser(start, tokenIndex);
			coutfile.close();
		}	


	}
	else {
		cout << "Can't open the file: " << inputFile << endl;
	}

	sourceFile.close();


	return 0;
}
