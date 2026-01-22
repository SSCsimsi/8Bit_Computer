
#include "main.h"

int main() {

	FILE *fptrCode;

	FILE *fptrCmd;
	FILE *fptrDat;
	FILE *fptrAdr;

	/* Open the code */
	ifstream codeFile("code.txt");

	/* Create the hex files for programming the computer */
	ofstream cmdFile("cmd.hex"); //cmd and dat in one file, since they are combined in one flash chip
	ofstream adrFile("adr.hex");

	string newLine;
	CreateList();

	getline(codeFile, newLine);
	for(Cmd cmd : Commands) {
		if(cmd.Compare(newLine)) {
			cmdFile << cmd.Put();
			//TODO: convert string to binary
			cmdFile << newLine.substr(4, 5);
		}

	}


	/* Close the files */
	codeFile.close();
	cmdFile.close();
	adrFile.close();
}
