/******************************************************************
*                                                                 *
*   Name: Daimeun Praytor                                         *
*   Simulates a subset of the MIPS instruction set                *
*                                                                 *
*   Compile: Compile with make, run with "sim.exe test.obj"       *
*                                                                 *
******************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

// Structure to manipulate instructions
typedef struct{
    union {
        struct {
            unsigned funct: 6;
            unsigned shamt: 5;
            unsigned rd: 5;
            unsigned rt: 5;
            unsigned rs: 5;
            unsigned opcode: 6;
        } rformat;
        struct {
            unsigned immed: 16;
            unsigned rt: 5;
            unsigned rs: 5;
            unsigned opcode: 6;
        } iformat;
        struct {
            unsigned address: 26;
            unsigned opcode: 6;
        } jformat;
        unsigned int encodedValue;
    } u;

    char format;
} mips_template;

// Correlates the register decimal value to the register
string registerTable(unsigned int registerDec);

// Prints the registers and their values.
void printRegs(vector<pair<string, int>> regs);

// Formats and prints the given instruction to the output file
void printInst(vector<string> inst);

// Prints data, formated for the initial log output
void printData(vector<int> dataVec, int numInst);

// Prints the altered data
void printAltData(vector<int> dataVec);

// Sets register values
void setReg(string regName, int regVal);

// Returns the register value
int getRegVal(string regName);

// Decodes a vector of hex instructions
void decode(vector<mips_template> hexInst, vector<vector<string>> &decodedInst);

// Simulates a list of decoded instructions
void simulate(vector<vector<string>> decodedInst, vector<int> dataVec, int numInst, int numData);

// Holds all register values.
vector<pair<string, int>> regs = {{"$zero", 0}, {"$at", 0}, {"$v0", 0}, {"$v1", 0},
                        {"$a0", 0}, {"$a1", 0}, {"$a2", 0}, {"$a3", 0},
                        {"$t0", 0}, {"$t1", 0}, {"$t2", 0}, {"$t3", 0},
                        {"$t4", 0}, {"$t5", 0}, {"$t6", 0}, {"$t7", 0},
                        {"$s0", 0}, {"$s1", 0}, {"$s2", 0}, {"$s3", 0},
                        {"$s4", 0}, {"$s5", 0}, {"$s6", 0}, {"$s7", 0},
                        {"$t8", 0}, {"$t9", 0}, {"$k0", 0}, {"$k1", 0},
                        {"$gp", 0}, {"$sp", 0}, {"$fp", 0}, {"$ra", 0},
                        {"$lo", 0}, {"$hi", 0}};

// Initializes the output stream
ofstream fout;

int main(int argc, char *argv[]) {

    // Vector for hex instructions.
    vector<mips_template> hexInst;

    // Decoded instructions
    vector<vector<string>> decodedInst;

    // Vector for data.
    vector<int> dataVec;
    
    // Sets input/output streams
    fout.open("log.txt");
    ifstream fin(argv[1]);

    // Gets the first line of the input file stream.
    string firstLine;
    getline(fin, firstLine);
    int numInst, numData;
    sscanf(firstLine.c_str(), "%d %d", &numInst, &numData);

    // Sets the global pointer
    setReg("$gp", numInst);

    // Stores the input
    string line;

    // Puts the instructions in a vector
    for(int i = 0; i < numInst; i++) {
        getline(fin, line);
        int val;
        if (sscanf(line.c_str(), "%x", &val) != 1)
            exit(-1);
        mips_template x;
        x.u.encodedValue = val;
        hexInst.push_back(x);
    }

    // Puts the data into a vector
    for(int i = 0; i < numData; i++) {
        getline(fin, line);
        int val;
        if (sscanf(line.c_str(), "%x", &val)!=1)
            exit(-1);
        dataVec.push_back(val);
    }

    decode(hexInst, decodedInst);

    // Prints all of the instructions
    fout << "insts:" << endl;
    for(int i = 0; i < decodedInst.size(); i++) {
        fout.width(4);
        fout << right << i << ": ";

        printInst(decodedInst[i]);
    }
    fout << "\n";

    printData(dataVec, numInst);
    simulate(decodedInst, dataVec, numInst, numData);

    fout.close();

    return 0;
}

// Decodes the instruction
void decode(vector<mips_template> hexInst, vector<vector<string>> &decodedInst) {
    string stringRs;
    string stringRt;
    string stringRd;
    string stringImmed;
    
    // Converts hex instructions into regular instructions.
    for(int i = 0; i < hexInst.size(); i++) {

        // R format
        if(hexInst[i].u.rformat.opcode == 0) {

            // Syscall
            if(hexInst[i].u.rformat.funct == 12){
                decodedInst.push_back({"syscall"});
            }
            // Mfhi
            else if(hexInst[i].u.rformat.funct == 16) {
                stringRd = registerTable(hexInst[i].u.rformat.rd);

                decodedInst.push_back({"mfhi", stringRd});
            }
            // Mflo
            else if(hexInst[i].u.rformat.funct == 18) {
                stringRd = registerTable(hexInst[i].u.rformat.rd);

                decodedInst.push_back({"mflo", stringRd});
            }
            // Mult
            else if(hexInst[i].u.rformat.funct == 24) {
                stringRs = registerTable(hexInst[i].u.rformat.rs);
                stringRt = registerTable(hexInst[i].u.rformat.rt);

                decodedInst.push_back({"mult", stringRs, stringRt});
            }
            // Div
            else if(hexInst[i].u.rformat.funct == 26) {
                stringRs = registerTable(hexInst[i].u.rformat.rs);
                stringRt = registerTable(hexInst[i].u.rformat.rt);

                decodedInst.push_back({"div", stringRs, stringRt});
            }
            // Addu
            else if(hexInst[i].u.rformat.funct == 33) {
                stringRs = registerTable(hexInst[i].u.rformat.rs);
                stringRt = registerTable(hexInst[i].u.rformat.rt);
                stringRd = registerTable(hexInst[i].u.rformat.rd);

                decodedInst.push_back({"addu", stringRd, stringRs, stringRt});
            }
            // Subu
            else if(hexInst[i].u.rformat.funct == 35) {
                stringRs = registerTable(hexInst[i].u.rformat.rs);
                stringRt = registerTable(hexInst[i].u.rformat.rt);
                stringRd = registerTable(hexInst[i].u.rformat.rd);

                decodedInst.push_back({"subu", stringRd, stringRs, stringRt});
            }
            // And
            else if(hexInst[i].u.rformat.funct == 36) {
                stringRs = registerTable(hexInst[i].u.rformat.rs);
                stringRt = registerTable(hexInst[i].u.rformat.rt);
                stringRd = registerTable(hexInst[i].u.rformat.rd);

                decodedInst.push_back({"and", stringRd, stringRs, stringRt});
            }
            // Or
            else if(hexInst[i].u.rformat.funct == 37) {
                stringRs = registerTable(hexInst[i].u.rformat.rs);
                stringRt = registerTable(hexInst[i].u.rformat.rt);
                stringRd = registerTable(hexInst[i].u.rformat.rd);

                decodedInst.push_back({"or", stringRd, stringRs, stringRt});
            }
            // Slt
            else if(hexInst[i].u.rformat.funct == 42) {
                stringRs = registerTable(hexInst[i].u.rformat.rs);
                stringRt = registerTable(hexInst[i].u.rformat.rt);
                stringRd = registerTable(hexInst[i].u.rformat.rd);

                decodedInst.push_back({"slt", stringRd, stringRs, stringRt});
            }
            // Invalid opcode
            else {
                stringImmed = to_string(hexInst[i].u.rformat.funct);
                cout << "Invalid Opcode/Function Line: " << i+1 << endl;
                exit(-1);
            }
        }

        // J format
        if(hexInst[i].u.jformat.opcode == 2) {
            stringImmed = to_string(hexInst[i].u.jformat.address);
            decodedInst.push_back({"j", stringImmed});
        }

        // I format
        if(hexInst[i].u.iformat.opcode > 3) {

            // Beq
            if(hexInst[i].u.iformat.opcode == 4){
                stringRs = registerTable(hexInst[i].u.iformat.rs);
                stringRt = registerTable(hexInst[i].u.iformat.rt);
                stringImmed = to_string(hexInst[i].u.iformat.immed);

                decodedInst.push_back({"beq", stringRs, stringRt, stringImmed});
            }
            // Bne
            else if(hexInst[i].u.iformat.opcode == 5){
                stringRs = registerTable(hexInst[i].u.iformat.rs);
                stringRt = registerTable(hexInst[i].u.iformat.rt);
                stringImmed = to_string(hexInst[i].u.iformat.immed);

                decodedInst.push_back({"bne", stringRs, stringRt, stringImmed});
            }
            // Addiu
            else if(hexInst[i].u.iformat.opcode == 9){
                stringRt = registerTable(hexInst[i].u.iformat.rt);
                stringRs = registerTable(hexInst[i].u.iformat.rs);
                stringImmed = to_string(hexInst[i].u.iformat.immed);

                decodedInst.push_back({"addiu", stringRt, stringRs, stringImmed});
            }
            // Lw
            else if(hexInst[i].u.iformat.opcode == 35){
                stringRt = registerTable(hexInst[i].u.iformat.rt);
                stringImmed = to_string(hexInst[i].u.iformat.immed);
                stringRs = registerTable(hexInst[i].u.iformat.rs);

                decodedInst.push_back({"lw", stringRt, stringImmed, stringRs});
            }
            // Sw
            else if(hexInst[i].u.iformat.opcode == 43){
                stringRt = registerTable(hexInst[i].u.iformat.rt);
                stringImmed = to_string(hexInst[i].u.iformat.immed);
                stringRs = registerTable(hexInst[i].u.iformat.rs);

                decodedInst.push_back({"sw", stringRt, stringImmed, stringRs});
            }
            else {
                cout << "Invalid Opcode Line: " << i+1 << endl;
                exit(-1);
            }
        }
        
    }
}

// Simulates the decoded instruction
void simulate(vector<vector<string>> decodedInst, vector<int> dataVec, int numInst, int numData) {
    int intRs;
    int intRt;
    int intRd;
    int intImmed;
    int intAddress;

    int quotient;
    int remainder;
    
    // Simulation loop
    for(int i = 0; i < numInst; i++){
        fout << "PC: " << i << endl;
        fout << "inst: ";
        printInst(decodedInst[i]);

        if(decodedInst[i][0] == "syscall") {
            int v0Val = getRegVal("$v0");
            

            // Prints the $a0 register
            if(v0Val == 1){
                int a0Val = getRegVal("$a0");
                cout << a0Val << endl;
            }
            // Sets $v0 register to the users input
            else if(v0Val == 5) {
                cout << "Syscall input: ";
                cin >> v0Val;
                setReg("$v0", v0Val);
            }
            // Exits the simulation
            else if(v0Val == 10) {
                fout << "exiting simulator" << endl;
                break;
            }
            else {
                cout << "Error: Invalid Syscall" << endl;
                exit(-1);
            }
        }
        else if(decodedInst[i][0] == "mfhi") {
            // Saves the $hi register
            intRd = getRegVal("$hi");

            // Sets the target register to the $hi registers value
            setReg(decodedInst[i][1], intRd);
        }
        else if(decodedInst[i][0] == "mflo") {
            // Saves the $lo register
            intRd = getRegVal("$lo");

            // Sets the target register to the $lo registers value
            setReg(decodedInst[i][1], intRd);
        }
        else if(decodedInst[i][0] == "mult") {
            long long product;
            intRs = getRegVal(decodedInst[i][1]);
            intRd = getRegVal(decodedInst[i][2]);

            product = (long long) intRs * (long long) intRd;

            // intRs is the $lo register
            intRs = (int)(product & 0xffffffff);

            // intRd is the $hi register
            intRd = (int)((product >> 32) & 0xffffffff);

            // Sets the $hi and $lo registers.
            setReg("$lo", intRs);
            setReg("$hi", intRd);

        }
        else if(decodedInst[i][0] == "div") {

            intRs = getRegVal(decodedInst[i][1]);
            intRt = getRegVal(decodedInst[i][2]);

            // If dividing by 0, error.
            if(intRs == 0 || intRt == 0) {
                cout << "Error: Cannot divide by 0 on PC " << i << endl;
                exit(-1);
            }

            // Gets the quotient and remainder.
            quotient = (intRs/intRt);
            remainder = (intRs%intRt);

            setReg("$hi", remainder);
            setReg("$lo", quotient);

        }
        else if(decodedInst[i][0] == "addu") {
            intRs = getRegVal(decodedInst[i][2]);
            intRt = getRegVal(decodedInst[i][3]);

            intRd = intRs + intRt;

            setReg(decodedInst[i][1], intRd);

        }
        else if(decodedInst[i][0] == "subu") {
            intRs = getRegVal(decodedInst[i][2]);
            intRt = getRegVal(decodedInst[i][3]);

            intRd = intRs - intRt;

            setReg(decodedInst[i][1], intRd);
        }
        else if(decodedInst[i][0] == "and") {
            intRs = getRegVal(decodedInst[i][2]);
            intRt = getRegVal(decodedInst[i][3]);

            intRd = (intRs & intRt);

            setReg(decodedInst[i][1], intRd);
        }
        else if(decodedInst[i][0] == "or") {
            intRs = getRegVal(decodedInst[i][2]);
            intRt = getRegVal(decodedInst[i][3]);

            intRd = (intRs | intRt);

            setReg(decodedInst[i][1], intRd);
        }
        else if(decodedInst[i][0] == "slt") {
            intRs = getRegVal(decodedInst[i][2]);
            intRt = getRegVal(decodedInst[i][3]);

            if(intRs < intRt) {
                setReg(decodedInst[i][1], 1);
            }
            else {
                setReg(decodedInst[i][1], 0);
            }
        }
        else if(decodedInst[i][0] == "j") {
            intAddress = stoi(decodedInst[i][1]);

            // Checks that the PC address is valid.
            if(intAddress >= 0 && intAddress < numInst) {
                i = intAddress - 1;
            }
            else {
                cout << "Error: Invalid Jump Address at PC " << i << endl;
                exit(-1);
            }
        }
        else if(decodedInst[i][0] == "beq") {
            intRs = getRegVal(decodedInst[i][1]);
            intRt = getRegVal(decodedInst[i][2]);
            intAddress = stoi(decodedInst[i][3]);

            // Branch if registers are equal
            if(intRs == intRt) {
                intAddress = i + intAddress;

                // Checks that the PC address is valid.
                if(intAddress >= 0 && intAddress < numInst) {
                    i = intAddress - 1;
                }
                else {
                    cout << "Error: Invalid Branch Address at PC " << i << endl;
                    exit(-1);
                }
            }
        }
        else if(decodedInst[i][0] == "bne") {
            intRs = getRegVal(decodedInst[i][1]);
            intRt = getRegVal(decodedInst[i][2]);
            intAddress = stoi(decodedInst[i][3]);

            // Branch if registers are not equal
            if(intRs != intRt) {
                intAddress = i + intAddress;

                // Checks that the PC address is valid.
                if(intAddress >= 0 && intAddress < numInst) {
                    i = intAddress - 1;
                }
                else {
                    cout << "Error: Invalid Branch Address at PC " << i << endl;
                    exit(-1);
                }
            }
        }
        else if(decodedInst[i][0] == "addiu") {
            intRs = getRegVal(decodedInst[i][2]);
            intImmed = stoi(decodedInst[i][3]);

            intRt = intRs + intImmed;
            setReg(decodedInst[i][1], intRt);
        }
        else if(decodedInst[i][0] == "lw") {
            intAddress = stoi(decodedInst[i][2]);
            intRs = getRegVal(decodedInst[i][3]);

            // Gets the target data PC address.
            intAddress = intAddress + intRs;
            intAddress = intAddress - numInst;
            
            // Checks that the target address is valid
            if(intAddress < numData && intAddress >= 0){
                setReg(decodedInst[i][1], dataVec[intAddress]);
            }
            else {
                cout << "Error: Invalid Data Address at PC " << i << endl;
                exit(-1);
            }

        }
        else if(decodedInst[i][0] == "sw") {
            intRt = getRegVal(decodedInst[i][1]);
            intAddress = stoi(decodedInst[i][2]);
            intRs = getRegVal(decodedInst[i][3]);

            // Gets the target data PC address.
            intAddress = intAddress + intRs;
            intAddress = intAddress - numInst;

            // Checks that the target address is valid
            if(intAddress < numData && intAddress >= 0){
                dataVec[intAddress] = intRt;
            }
            else {
                cout << "Error: Invalid Data Address at PC " << i << endl;
                exit(-1);
            }
        }

        printRegs(regs);
        printAltData(dataVec);
    }
}

void printAltData(vector<int> dataVec) {
    fout << "data memory:" << endl;
    int dataPerLine = 0;

    for(int i = 0; i < dataVec.size(); i++) {

        // If its the 4th data segement on line, newline.
        if(dataPerLine == 3) {
            dataPerLine = 0;
            fout << "\n";
        }

        fout.width(8);
        fout << right << "data[";

        fout.width(3);
        fout << right << i;
        fout << "] =";
        fout.width(6);
        fout << dataVec[i];
        ++dataPerLine;
    }

    fout << "\n\n\n";

}

void printData(vector<int> dataVec, int numInst) {
    fout << "data:" << endl;

    for(int i = 0; i < dataVec.size(); i++) {
        fout.width(4);
        fout << right << numInst << ": " << dataVec[i] << endl;
        ++numInst;
    }

    fout << "\n";
}

void printInst(vector<string> inst) {

    // Special case for lw and sw
    if(inst[0] == "lw" || inst[0] == "sw") {
        fout.width(10);
        fout << left << inst[0];
        fout << inst[1] << "," << inst[2] << "(" << inst[3] << ")" << endl;
    }
    else if(inst[0] == "j") {
        fout << inst[0] << " " << inst[1] << endl;
    }
    else if(inst[0] == "syscall") {
        fout << inst[0] << endl;
    }
    else{
        fout.width(10);
        fout << left << inst[0];

        for(int j = 1; j < inst.size(); j++) {
                
            // If the last register add newline
            if(j == inst.size()-1){
                fout << inst[j] << endl;
            }
            else {
                fout << inst[j] << ",";
            }
        }
    }

    return;
}


void printRegs(vector<pair<string, int>> regs){
    // Tracks the number of instructions for each line.
    int numInLine = 0;

    fout << "\nregs:" << endl;

    for(int i = 0; i < regs.size(); i++) {

        // If its the 5th register on a line, go to next line.
        if(numInLine == 4) {
            numInLine = 0;
            fout << "\n";
        }

        fout.width(8);
        fout << right << regs[i].first;
        fout << " =";
        fout.width(6);
        fout << right << regs[i].second;

        numInLine++;
    }

    fout << "\n\n";

}

void setReg(string regName, int regVal){
    if(regName == "$zero") {
        exit(-1);
    }
    for(int i = 0; i < regs.size(); i++) {
        if(regName == regs[i].first) {
            regs[i].second = regVal;
        }
    }
}

int getRegVal(string regName) {
    int regVal;

    for(int i = 0; i < regs.size(); i++) {
        if(regName == regs[i].first) {
            regVal = regs[i].second;
            break;
        }
    }

    return regVal;
}

// Returns the register name
string registerTable(unsigned int registerDec){
    switch(registerDec) {
        case 0:
            return "$zero";
        case 1:
            return "$at";
        case 2:
            return "$v0";
        case 3:
            return "$v1";
        case 4:
            return "$a0";
        case 5:
            return "$a1";
        case 6:
            return "$a2";
        case 7:
            return "$a3";
        case 8:
            return "$t0";
        case 9:
            return "$t1";
        case 10:
            return "$t2";
        case 11:
            return "$t3";
        case 12:
            return "$t4";
        case 13:
            return "$t5";
        case 14:
            return "$t6";
        case 15:
            return "$t7";
        case 16:
            return "$s0";
        case 17:
            return "$s1";
        case 18:
            return "$s2";
        case 19:
            return "$s3";
        case 20:
            return "$s4";
        case 21:
            return "$s5";
        case 22:
            return "$s6";
        case 23:
            return "$s7";
        case 24:
            return "$t8";
        case 25:
            return "$t9";
        case 26:
            return "$k0";
        case 27:
            return "$k1";
        case 28:
            return "$gp";
        case 29:
            return "$sp";
        case 30:
            return "$fp";
        case 31:
            return "$ra";

        cout << "Error: Invalid register" << endl;
        exit(-1);
    }
    
    return "error";
}
