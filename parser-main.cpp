//#include "stdafx.h"
#include "globals.h"
#include "lexer.h"
#include "parser.h"

using namespace std;

string outputFile;

int main(int argc, const char * argv[]) {
	string inputFile, word;
	vector<string> allCleanWords;

	//test sample source code
	cout << "Enter test file name(format: filename.txt): ";
	cin >> inputFile;

	cout << "Enter test output file name(format: filename.txt): ";
	cin >> outputFile;

	fstream coutfile;
	coutfile.open(outputFile, fstream::out);

	ifstream sourceFile(inputFile);

	if (sourceFile.is_open()) {
		//create allWords
		while (sourceFile >> word) {
			vector<string> vec = splitWord(word);
			for (unsigned i = 0; i < vec.size(); ++i) {
				if (!vec.at(i).empty())
					allCleanWords.push_back(vec.at(i));
			}
		}
		allCleanWords.push_back("$");	//push "$" after read in all the words

		Parser(allCleanWords);
	}
	else {
		cout << "Can't open the file: " << inputFile << endl;
	}

	printInstr();
	printSymbol();
	sourceFile.close();
	coutfile.close();

	return 0;
}
