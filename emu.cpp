/*
TITLE: Source code for Emulator(C++)
Name: Parv Gadhethariya
ROLL NO: 2201CS91
Declaration of Authorship
The emu.cpp file, written in C++, is a component of the CS210 miniproject.
*/

#include <iostream>    
#include <vector>      
#include <algorithm>   
#include <cmath>       
#include <string>      
#include <map>         
#include <set>                     
#include <iomanip>     
#include <functional>  
#include <sstream>    
#include <fstream>
#include <bitset> 
using namespace std;
#define uint unsigned int
#define dash cout << "-------------------------------\n";
// registers
int SP = 0, A = 0, B = 0, PC = 0;

// memory
vector<uint> memory(16777216,19);



// decimal to hex
string make_hex(uint decimal)
{
    stringstream hex_stream;
    hex_stream << setfill('0') << setw(8) << hex << decimal;
    return hex_stream.str();
}

//Execution Function Table
void execute_function(int mem_code, int operand) {
    if (mem_code == 0) { // ldc
        B = A;
        A = operand;
    } else if (mem_code == 1) { // adc
        A += operand;
    } else if (mem_code == 2) { // ldl
        B = A;
        A = memory[SP + operand];
    } else if (mem_code == 3) { // stl
        memory[SP + operand] = A;
        A = B;
    } else if (mem_code == 4) { // ldnl
        A = memory[A + operand];
    } else if (mem_code == 5) { // stnl
        memory[A + operand] = B;
    } else if (mem_code == 6) { // add
        A += B;
    } else if (mem_code == 7) { // sub
        A = B - A;
    } else if (mem_code == 8) { // shl
        A = B << A;
    } else if (mem_code == 9) { // shr
        A = B >> A;
    } else if (mem_code == 10) { // adj
        SP += operand;
    } else if (mem_code == 11) { // a2sp
        SP = A;
        A = B;
    } else if (mem_code == 12) { // sp2a
        B = A;
        A = SP;
    } else if (mem_code == 13) { // call
        B = A;
        A = PC;
        PC += operand;
    } else if (mem_code == 14) { // return
        PC = A;
        A = B;
    } else if (mem_code == 15) { // brz
        if (A == 0) {
            PC += operand;
        }
    } else if (mem_code == 16) { // brlz
        if (A < 0) {
            PC += operand;
        }
    } else if (mem_code == 17) { // br
        PC += operand;
    }
}

// Reading the .o file
uint read_file(fstream &input_file)
{
    input_file.seekg(0, ios::end);
    uint size = input_file.tellg();
    input_file.seekg(0, ios::beg);
    size/=4;
    uint lines = size;

    uint op;
    for (uint i = 0; i < lines; i++)
    {
        input_file.read(reinterpret_cast<char*>(&op), sizeof(op));
        memory[i] = op;
    }

    return lines;
}

// printing the memory dump
void print_memory_dump(uint line, bool is_before)
{
    cout<<(is_before)? "Memory dump before execution":"Memory dump after execution";
    cout<<endl;
    for (uint i = 0; i < line; i++){
        if (i % 4 == 0)
        {
            cout << "\n0x" << make_hex(i);
        }
        cout << "\t0x" << make_hex(memory[i]);
    }
    cout << endl;
    return;
}

// Printing isa
void print_isa()
{
    std::vector<std::string> opd = {"value","value","offset","offset","offset",
    "offset","","","","","value","","","offset","","offset","offset","offset",""};
    vector<pair<string,int>> mnemonic_opp = {
        {"ldc", 0},
        {"adc", 1},
        {"ldl", 2},
        {"stl", 3},
        {"ldnl", 4},
        {"stnl", 5},
        {"add", 6},
        {"sub", 7},
        {"shl", 8},
        {"shr", 9},
        {"adj", 10},
        {"a2sp", 11},
        {"sp2a", 12},
        {"call", 13},
        {"return", 14},
        {"brz", 15},
        {"brlz", 16},
        {"br", 17},
        {"HALT", 18}};

    cout << "Opcode\tMnemonic\tOperand\n";
    dash

    for (int i = 0; i < 19; i++)
    {
        cout << i << "\t" << mnemonic_opp[i].first << "\t\t" << opd[mnemonic_opp[i].second] << endl;
    }

    dash

    return;
}

int execute(bool trace)
{
    SP = (1 << 23) - 1;
    uint mem_code = 0, operand_l = 0, instr_no = 0;
    int operand = 0;

    // For valid memcodes
    while (mem_code <= 18)
    {
        // extract mem_code (last 8 bits)
        mem_code = memory[PC] % (1<<8);
        if (mem_code == 18){
            break;
        }
        else if (mem_code > 18){
            return 1;
        }

        // extract operand (first 24 bits)
        operand_l = memory[PC] >> 8;
        (operand_l > 8388607)? operand = (int)operand_l - 16777216:operand = (int)operand_l;
        
        if (trace)
        {
            cout << "PC = 0x" << make_hex(PC)<< "\tSP = 0x" << make_hex(SP)
            << "\tA = 0x" << make_hex(A)<< "\tB = 0x" << make_hex(B)
            <<endl;
        }

        execute_function(mem_code, operand);

        PC++;
        instr_no++;
    }

    cout << endl;
    cout << instr_no << " instructions executed." << endl;

    return 0;
}

// Define a map to associate strings with an enum
enum ArgumentType { TRACE, BEFORE, AFTER, ISA, INVALID };
map<string, ArgumentType> argumentMap = {
    {"-trace", TRACE},
    {"-before", BEFORE},
    {"-after", AFTER},
    {"-isa", ISA}
};

// Function to convert string to ArgumentType
ArgumentType getArgumentType(const std::string& arg) {
    auto it = argumentMap.find(arg);
    if (it != argumentMap.end()) {
        return it->second;
    }
    return INVALID;
}

int main(int argc, char **argv)
{
    // Validating arguments
    if (argc != 3)
    {
        cout << "Format error detected.\n"
        "Correct command: emu [option] object_file.o\n"
        "-trace\t:\tDisplays the sequence of instructions.\n"
        "-before\t:\tPresents memory state prior to execution.\n"
        "-after\t:\tPresents memory state post-execution.\n"
        "-isa\t:\tShows the Instruction Set Architecture.";
        return 1;
    }

    // Checking and setting attributes
    bool trace = false, before = false, after = false, isa = false;
    uint lines;
    string argument = string(argv[1]);

    ArgumentType argType = getArgumentType(argument);
    switch(argType) {
        case TRACE:
            trace = true;
            break;
        case BEFORE:
            before = true;
            break;
        case AFTER:
            after = true;
            break;
        case ISA:
            isa = true;
            break;
        default:
            cout << "Format error detected.\n"
            "Correct command: emu [option] object_file.o\n"
            "-trace\t:\tDisplays the sequence of instructions.\n"
            "-before\t:\tPresents memory state prior to execution.\n"
            "-after\t:\tPresents memory state post-execution.\n"
            "-isa\t:\tShows the Instruction Set Architecture.";
            return 1;
    }

    fstream input_file(string(argv[2]), ios::binary | ios::in);

    // Reading file
    if (input_file.is_open())
    {
        input_file.seekg(0, ios::end);
        // Check if the file pointer is at the beginning (which means the file is empty)
        if (input_file.tellg() == 0)
        {
            cout << "ERROR: Empty file.";
            input_file.close();
            return 1;
        }
        else{
            lines = read_file(input_file);
            input_file.close();
        }
    }
    else
    {
        cout << "ERROR: Empty file.";
        return 1;
    }

    // before mem dump
    if(before){
        print_memory_dump(lines, true);
    }
    // isa
    else if (isa){
        print_isa();
    }

    // executing and trace
    int err = execute(trace);

    if (err == 1){
        cout << "ERROR : Incorrect mnemonic encountered.";
        return 1;
    }

    // after mem dump
    if (after){
        cout<<"Memory dump after execution"<<endl;
        for (uint i=0; i<lines; i++){
            if (i % 4 == 0){
                cout << "\n0x" << make_hex(i);
            }
            cout << "\t0x" << make_hex(memory[i]);
        }

        cout << endl;
    }

    ofstream memdump_file;
    string memdump_file_name = string(argv[2]);
    memdump_file_name.append("_memdump.txt");
    memdump_file.open(memdump_file_name, ios::out);
    // printing mem dump to file
    for (uint i = 0; i < lines; i++){
        if (i % 4 == 0)
        {
            memdump_file << "\n0x" << make_hex(i);
        }
        memdump_file << "\t0x" << make_hex(memory[i]);
    }
    memdump_file<< endl;

    memdump_file.close();

    return 0;
}