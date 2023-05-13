#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <ctype.h>
#include "Lex.h"
#include "Par.h"

using namespace std;

int main() {
	ifstream inFile;
	ofstream outFile("output.txt");
	string filename;
	string line;
	do {
		cout << "Enter a file name .txt (or 'exit' to quit): ";
		cin >> filename;

		if (!(filename == "exit"))
		{
			inFile.open(filename.c_str());

			//Make sure file is opened
			if (!inFile.is_open())
			{
				cerr << "Failed to open the file.\n";
				cerr << "Please enter correct file.\n";
				system("pause");
				exit(1);
			}
			else
			{
				outFile << "SOURCE CODE:\n\n";
				while (getline(inFile, line))
				{
					outFile << line << endl;
				}
			}

			outFile << "\n\nOUTPUT:\n";
			outFile << endl;

			Par check;

			inFile.clear();
			inFile.seekg(0);

			//run top-down RDP parser
			check.RAT23S(inFile, outFile);
			check.printSym(inFile, outFile);
			check.printInstr(inFile, outFile);
			inFile.close();

			check.~Par();
			cout << endl;
			outFile << endl;
		}

	} while (!(filename == "exit"));

	outFile.close();
	cout << endl;
	return 0;
}

