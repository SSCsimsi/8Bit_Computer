
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <string>
#include <list>

using namespace std;

#define CMDLENGTH 3

void CreateList();

class Cmd {
public:

	string name;
	uint8_t val;

	/*
	 * the name is made up of 3 ascii symbols,
	 * the value has to fit within 8 bits
	 */
	void Init(string nameIn, uint8_t valIn) {
		name = nameIn;
		val = valIn;
	}

	uint8_t Compare(string newLine) {
		return name == newLine.substr(0, CMDLENGTH);
	}

	uint8_t Put() {
		return val;
	}


private:

};

list<Cmd> Commands;

int main() {

	Cmd Nop;
	Nop.Init("nop", 0x00);
	Commands.push_back(Nop);

	Cmd Jump;
	Jump.Init("jmp", 0x01);
	Commands.push_back(Jump);

	Cmd JumpIf;
	JumpIf.Init("cjp", 0x02);
	Commands.push_back(JumpIf);

	/* Open the code */
	ifstream codeFile("code.txt");

	/* Create the hex files for programming the computer */
	ofstream cmdFile("cmd.hex"); //cmd and dat in one file, since they are combined in one flash chip
	ofstream adrFile("adr.hex");

	string newLine;
	uint8_t check;

	while(getline(codeFile, newLine)) {

		check = 0;

		for(Cmd cmd : Commands) {
			if(cmd.Compare(newLine)) {

				cmdFile << cmd.Put();
				check = 1;

				break;
			}
		}

		if(!check) {
			ofstream Error("UnknownCommand.txt");
			Error.close();

			return -1;
		}

		cmdFile << (uint8_t)stoi(newLine.substr(4, 5)); //value changed from string to int
		adrFile << (uint8_t)stoi(newLine.substr(7,10));

		getline(codeFile, newLine);
	}

	/* Close the files */
	codeFile.close();
	cmdFile.close();
	adrFile.close();
}



void CreateList() {


}
