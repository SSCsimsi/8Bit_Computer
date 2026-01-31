
#include <cstdint>
#include <iostream>
#include <fstream>
#include <string>
#include <list>

using namespace std;

#define CMDLENGTH 3

class Cmd {
public:

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
	string name;
	uint8_t val;
};

list<Cmd> Commands;

void CreateList();

int main() {

	CreateList();

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

				uint8_t b = cmd.Put();
				cmdFile.write(reinterpret_cast<char*>(&b), 1);

				check = 1;
				break;
			}
		}

		if(!check) {
			ofstream Error("UnknownCommand.txt");
			Error.close();

			return -1;
		}

		uint8_t val = stoi(newLine.substr(4,2));
		cmdFile.write(reinterpret_cast<char*>(&val), 1);

		uint8_t adr = stoi(newLine.substr(7,4));
		adrFile.write(reinterpret_cast<char*>(&adr), 1);
	}

	/* Close the files */
	codeFile.close();
	cmdFile.close();
	adrFile.close();
}

void CreateList() {

    Cmd cmd;

    // --- CORE ---
    cmd.Init("nop", 0x00); Commands.push_back(cmd); // No Operation
    cmd.Init("jmp", 0x01); Commands.push_back(cmd); // Jump
    cmd.Init("cjp", 0x02); Commands.push_back(cmd); // Jump if

    // --- BUS TRANSFER ---
    cmd.Init("sti", 0x03); Commands.push_back(cmd); // Store to interface reg 1
    cmd.Init("stj", 0x04); Commands.push_back(cmd); // Store to interface reg 2
    cmd.Init("ldi", 0x05); Commands.push_back(cmd); // Load interface regs

    // --- STORAGE ---
    cmd.Init("sto", 0x06); Commands.push_back(cmd); // Store to RAM
    cmd.Init("lod", 0x07); Commands.push_back(cmd); // Load from RAM

    // --- ALU ---
    cmd.Init("add", 0x08); Commands.push_back(cmd); // Add
    cmd.Init("adc", 0x09); Commands.push_back(cmd); // Add with carry
    cmd.Init("sub", 0x0A); Commands.push_back(cmd); // Subtract

    // --- INCREMENTOR ---
    cmd.Init("inc", 0x0B); Commands.push_back(cmd); // Increment
    cmd.Init("dec", 0x0C); Commands.push_back(cmd); // Decrement

    // --- LOGIC ---
    cmd.Init("bnt", 0x0D); Commands.push_back(cmd); // Binary Not
    cmd.Init("bor", 0x0E); Commands.push_back(cmd); // Binary Or
    cmd.Init("ban", 0x0F); Commands.push_back(cmd); // Binary And
    cmd.Init("bxr", 0x10); Commands.push_back(cmd); // Binary Xor
    cmd.Init("not", 0x11); Commands.push_back(cmd); // Bitwise Not
    cmd.Init("orr", 0x12); Commands.push_back(cmd); // Bitwise Or
    cmd.Init("and", 0x13); Commands.push_back(cmd); // Bitwise And
    cmd.Init("xor", 0x14); Commands.push_back(cmd); // Bitwise Xor

    // --- COMPARE ---
    cmd.Init("cgt", 0x15); Commands.push_back(cmd); // Compare Greater than
    cmd.Init("ceq", 0x16); Commands.push_back(cmd); // Compare Equal
    cmd.Init("clt", 0x17); Commands.push_back(cmd); // Compare Smaller than

    // --- A/B/C REGISTERS ---
    cmd.Init("sta", 0x18); Commands.push_back(cmd); // Store A
    cmd.Init("lda", 0x19); Commands.push_back(cmd); // Load A
    cmd.Init("stb", 0x1A); Commands.push_back(cmd); // Store B
    cmd.Init("ldb", 0x1B); Commands.push_back(cmd); // Load B
    cmd.Init("stc", 0x1C); Commands.push_back(cmd); // Store C
    cmd.Init("ldc", 0x1D); Commands.push_back(cmd); // Load C

    // --- FLAG REGISTER ---
    cmd.Init("stf", 0x1E); Commands.push_back(cmd); // Store Flags
    cmd.Init("lcr", 0x1F); Commands.push_back(cmd); // Load Carry
    cmd.Init("ldo", 0x20); Commands.push_back(cmd); // Load Overflow
    cmd.Init("ldf", 0x21); Commands.push_back(cmd); // Load Flags

    // --- SHIFT REGISTER ---
    cmd.Init("std", 0x22); Commands.push_back(cmd); // Store Distance
    cmd.Init("ldd", 0x23); Commands.push_back(cmd); // Load Distance
    cmd.Init("sft", 0x24); Commands.push_back(cmd); // Shift
    cmd.Init("sfc", 0x25); Commands.push_back(cmd); // Shift Circular

    // --- INPUTS ---
    cmd.Init("sti", 0x26); Commands.push_back(cmd); // Store Inputs
    cmd.Init("ldi", 0x27); Commands.push_back(cmd); // Load Inputs
    cmd.Init("ldu", 0x28); Commands.push_back(cmd); // Load Unread

    // --- OUTPUTS ---
    cmd.Init("sop", 0x29); Commands.push_back(cmd); // Store Outputs
    cmd.Init("lop", 0x2A); Commands.push_back(cmd); // Load Outputs


    /* unused, since we did not build the screen

    // --- SCREEN ---
    cmd.Init("str", 0x2B); Commands.push_back(cmd); // Store Row
    cmd.Init("ldr", 0x2C); Commands.push_back(cmd); // Load Row

    // --- CONTROLLER ---
    cmd.Init("lin", 0x2D); Commands.push_back(cmd); // Load Inputs

    */
}


