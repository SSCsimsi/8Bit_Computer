
#include <cstdint>
#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <bits/stdc++.h>


using namespace std;

#define CMDLENGTH 3

enum Requires{  
    Nothing,
    Data,
    Address,
    Both
};

enum Disables{
    None,
    DataBus,
    AddressBus
};

class Cmd {
public:

	/*
	 * the name is made up of 3 ascii symbols,
	 * the value has to fit within 8 bits
	 */
	void Init(string nameIn, uint8_t valIn, Requires reqIn, Disables disIn) {
		name = nameIn;
		val = valIn;
        req = reqIn;
        dis = disIn;
	}

	string Name() {
		return name;
	}

	uint8_t Put() {
		return val;
	}

    uint8_t Req(Requires reqIn) {
        return (req == reqIn);
    }

    Disables Dis() {
        return dis;
    }

private:
    string name;
	uint8_t val;
    Requires req;
    Disables dis;
};

list<Cmd> Commands;

void CreateList(void);
string ToHex(uint16_t num);

int main() {

	CreateList();

	/* Open the code */
	ifstream codeFile("code.txt");

	/* Create the hex files for programming the computer */
	ofstream cmdFile("cmd", ios::binary); //cmd and dat in one file, since they are combined in one flash chip
	ofstream adrFile("adr", ios::binary);

    /* Create a txt for logging errors */
    ofstream errorLog("errors.txt");

	string newLine;
	uint8_t check;

    uint16_t currentLine = 0;

    Disables prevBusAccess = None;

	while(getline(codeFile, newLine)) {

        check = 0;

        /* cut off the line number */
        uint8_t pos = newLine.find(' ') + 1;
        newLine = newLine.substr(pos);

        /* in order to avoid an empty string later */
        newLine = newLine.append("               ");

		for(Cmd cmd : Commands) {

			if(cmd.Name() == newLine.substr(0, newLine.find(' '))) {

                /* write cmd value to the file */
                uint8_t b = cmd.Put();
                cmdFile.write(reinterpret_cast<char*>(&b), 1);

                

                /* cmd XX ---- */
                uint8_t val = 0x00;

                /* only if the command needs data, and no previous command is outputting data to the bus */
                if((cmd.Req(Requires::Data) or cmd.Req(Requires::Both)) and !(prevBusAccess == Disables::DataBus)) {

                    /* try to write 8 data bits to the cmd file */
                    try{
                        newLine = newLine.substr(newLine.find(' ') + 1);
                        string s = newLine.substr(0, 2);
                        val = stoul(s, nullptr, 16);
                    }
                    catch(exception e) {
                        errorLog << "unknown data value at line: " << ToHex(currentLine) << endl;
                    }
                }
                cmdFile.write(reinterpret_cast<char*>(&val), 1);



                /* cmd -- XXXX */
                uint16_t adr = 0x0000;

                /* only if the command needs an address, and no previous command is outputting an address to the bus */
                if((cmd.Req(Requires::Address) or cmd.Req(Requires::Both)) and !(prevBusAccess == Disables::AddressBus)) {

                    /* try to write 16bits to the adr file */
                    try{
                        newLine = newLine.substr(newLine.find(' ') + 1);
                        string s = newLine.substr(0, 4);
                        adr = stoul(s, nullptr, 16);
                    } 
                    catch(exception e) {
                        errorLog << "unknown address value at line: " << ToHex(currentLine) << endl;
                    }
                }
                adrFile.write(reinterpret_cast<char*>(&adr), 2);



                currentLine++;
                prevBusAccess = cmd.Dis();

                check = 1;
                break;
			}
		}

		if(!check) {

			errorLog << "unknown command at line: " << ToHex(currentLine) << endl;
			return -1;
		}
	}

	/* Close the files */
	codeFile.close();
	cmdFile.close();
	adrFile.close();
    errorLog.close();
}

void CreateList() {

    Cmd cmd;

    // --- CORE ---
    cmd.Init("nop", 0x00, Requires::Nothing, Disables::None);       Commands.push_back(cmd); // No Operation
    cmd.Init("jmp", 0x01, Requires::Address, Disables::None);       Commands.push_back(cmd); // Jump
    cmd.Init("cjp", 0x02, Requires::Both,    Disables::None);       Commands.push_back(cmd); // Jump if

    // --- BUS TRANSFER ---
    cmd.Init("stx", 0x03, Requires::Data,    Disables::None);       Commands.push_back(cmd); // Store to interface reg 1
    cmd.Init("sty", 0x04, Requires::Data,    Disables::None);       Commands.push_back(cmd); // Store to interface reg 2
    cmd.Init("ldz", 0x05, Requires::Nothing, Disables::AddressBus); Commands.push_back(cmd); // Load interface regs

    // --- STORAGE ---
    cmd.Init("sto", 0x06, Requires::Both,    Disables::None);       Commands.push_back(cmd); // Store to RAM
    cmd.Init("lod", 0x07, Requires::Address, Disables::DataBus);    Commands.push_back(cmd); // Load from RAM

    // --- ALU ---
    cmd.Init("add", 0x08, Requires::Nothing, Disables::DataBus);    Commands.push_back(cmd); // Add
    cmd.Init("adc", 0x09, Requires::Nothing, Disables::DataBus);    Commands.push_back(cmd); // Add with carry
    cmd.Init("sub", 0x0A, Requires::Nothing, Disables::DataBus);    Commands.push_back(cmd); // Subtract

    // --- INCREMENTOR ---
    cmd.Init("inc", 0x0B, Requires::Nothing, Disables::DataBus);    Commands.push_back(cmd); // Increment
    cmd.Init("dec", 0x0C, Requires::Nothing, Disables::DataBus);    Commands.push_back(cmd); // Decrement

    // --- LOGIC ---
    cmd.Init("not", 0x0D, Requires::Nothing, Disables::DataBus);    Commands.push_back(cmd); // Binary Not
    cmd.Init("orr", 0x0E, Requires::Nothing, Disables::DataBus);    Commands.push_back(cmd); // Binary Or
    cmd.Init("and", 0x0F, Requires::Nothing, Disables::DataBus);    Commands.push_back(cmd); // Binary And
    cmd.Init("xor", 0x10, Requires::Nothing, Disables::DataBus);    Commands.push_back(cmd); // Binary Xor
    cmd.Init("bnt", 0x11, Requires::Nothing, Disables::DataBus);    Commands.push_back(cmd); // Bitwise Not
    cmd.Init("bor", 0x12, Requires::Nothing, Disables::DataBus);    Commands.push_back(cmd); // Bitwise Or
    cmd.Init("ban", 0x13, Requires::Nothing, Disables::DataBus);    Commands.push_back(cmd); // Bitwise And
    cmd.Init("bxr", 0x14, Requires::Nothing, Disables::DataBus);    Commands.push_back(cmd); // Bitwise Xor

    // --- COMPARE ---
    cmd.Init("cgt", 0x15, Requires::Nothing, Disables::DataBus);    Commands.push_back(cmd); // Compare Greater than
    cmd.Init("ceq", 0x16, Requires::Nothing, Disables::DataBus);    Commands.push_back(cmd); // Compare Equal
    cmd.Init("clt", 0x17, Requires::Nothing, Disables::DataBus);    Commands.push_back(cmd); // Compare Smaller than

    // --- A/B/C REGISTERS ---
    cmd.Init("sta", 0x18, Requires::Data,    Disables::None);       Commands.push_back(cmd); // Store A
    cmd.Init("lda", 0x19, Requires::Nothing, Disables::DataBus);    Commands.push_back(cmd); // Load A
    cmd.Init("stb", 0x1A, Requires::Data,    Disables::None);       Commands.push_back(cmd); // Store B
    cmd.Init("ldb", 0x1B, Requires::Nothing, Disables::DataBus);    Commands.push_back(cmd); // Load B
    cmd.Init("stc", 0x1C, Requires::Data,    Disables::None);       Commands.push_back(cmd); // Store C
    cmd.Init("ldc", 0x1D, Requires::Nothing, Disables::DataBus);    Commands.push_back(cmd); // Load C

    // --- FLAG REGISTER ---
    cmd.Init("stf", 0x1E, Requires::Data,    Disables::None);       Commands.push_back(cmd); // Store Flags
    cmd.Init("lcr", 0x1F, Requires::Nothing, Disables::DataBus);    Commands.push_back(cmd); // Load Carry
    cmd.Init("ldo", 0x20, Requires::Nothing, Disables::DataBus);    Commands.push_back(cmd); // Load Overflow
    cmd.Init("ldf", 0x21, Requires::Nothing, Disables::DataBus);    Commands.push_back(cmd); // Load Flags

    // --- SHIFT REGISTER ---
    cmd.Init("std", 0x22, Requires::Data,    Disables::None);       Commands.push_back(cmd); // Store Distance
    cmd.Init("ldd", 0x23, Requires::Nothing, Disables::DataBus);    Commands.push_back(cmd); // Load Distance
    cmd.Init("sft", 0x24, Requires::Nothing, Disables::DataBus);    Commands.push_back(cmd); // Shift
    cmd.Init("sfc", 0x25, Requires::Nothing, Disables::DataBus);    Commands.push_back(cmd); // Shift Circular

    // --- INPUTS ---
    cmd.Init("sti", 0x26, Requires::Data,    Disables::None);       Commands.push_back(cmd); // Store Inputs
    cmd.Init("ldi", 0x27, Requires::Nothing, Disables::DataBus);    Commands.push_back(cmd); // Load Inputs
    cmd.Init("ldu", 0x28, Requires::Nothing, Disables::DataBus);    Commands.push_back(cmd); // Load Unread

    // --- OUTPUTS ---
    cmd.Init("sop", 0x29, Requires::Data,    Disables::None);       Commands.push_back(cmd); // Store Outputs
    cmd.Init("lop", 0x2A, Requires::Nothing, Disables::DataBus);    Commands.push_back(cmd); // Load Outputs



    /* unused, since we did not build the screen

    // --- SCREEN ---
    cmd.Init("str", 0x2B, Requires::Both,    Disables::None);       Commands.push_back(cmd); // Store Row
    cmd.Init("ldr", 0x2C, Requires::Nothing, Disables::DataBus);    Commands.push_back(cmd); // Load Row

    // --- CONTROLLER ---
    cmd.Init("lin", 0x2D, Requires::Nothing, Disables::DataBus);    Commands.push_back(cmd); // Load Inputs

    */
}

string ToHex(uint16_t num) {
    stringstream s;
    s << hex << (int)num;
    return s.str();
}