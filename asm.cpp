/*
TITLE: Source code for Assembler(C++)
AUTHOR: Parv Gadhethariya
ROLL NO: 2201CS91
Declaration of Authorship
The asm.cpp file, written in C++, is a component of the CS209/CS210 miniproject undertaken
at the Department of Computer Science and Engineering, IIT Patna.
*/

#include<bits/stdc++.h>
#include <sstream>
#include <fstream>
using namespace std;
#define ll long long

// Handles listing vector format
class Listings {
public:
    bool op_backpatch = false;
    int operand;
    int program_counter;
    int opcode;
    string instruction = "";
};

// mnemonics table
map<string,int> mnemonic = {
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
    {"HALT", 18},
    {"data", 19},
    {"SET", 20}
};

int PC = -1; // Program counter

bool is_num(char c){
    return (c>='0'&&c<='9');
}
bool is_small_alpha(char c){
    return (c>='a'&&c<='z');
}
bool is_capital_alpha(char c){
    return (c>='A'&&c<='Z');
}
bool is_useless(char c){
    return (c==(char)9||c==(char)32);
}
bool is_binary(char c){
    return (c=='0'||c=='1');
}
bool is_octal(char c){
    return (c>='0'&&c<='7');
}

// Used to store the listing file data
vector<Listings> listings;


// remove whitespace from both sides
string whitespace_remover(string s){
    int n = s.length();
    string ans = "";
    bool is_space_over=false;
    for (int i=0; i<n; i++){
        if (!(is_useless(s[i]))) is_space_over=true;
        if(is_space_over) ans+=s[i];
    }
    s=ans;
    n=s.length();
    ans="";
    is_space_over=false;
    for (int i=n-1; i>=0; i--){
        if (!(is_useless(s[i]))) is_space_over=true;
        if(is_space_over) ans+=s[i];
    }
    reverse(ans.begin(),ans.end());
    return ans;
}

// Check if it is possible to have the given label
bool is_possible_label(string s){
    int n = s.length();

    // Should not me a mnemonic
    if (mnemonic.find(s) != mnemonic.end()) return false;

    // Should not start with a number
    if (is_num(s[0])) return false;

    // Should be alphanumeric
    for (int i=0; i<n; i++){
        if (!(is_small_alpha(s[i])|| is_capital_alpha(s[i]) || is_num(s[i]))){
            return false;
        }
    }
    return true;
}

// Used to store errors, listing file lines respectively
set<string> errors;

string givelable(string line){
    // Checking for label
    size_t label_pos = string::npos;
    for (size_t i = 0; i < line.size(); ++i) {
        if (line[i] == ':') {
            label_pos = i;
            break;
        }
    }
    if (label_pos != string::npos){
        // Verifying if label is good, else adding error statemnts.
        string label = line.substr(0, label_pos);
        label = whitespace_remover(label);
        return label;
    }
    else return "";
}

string give_rem_line(string line){
    size_t label_pos = string::npos;
    for (size_t i = 0; i < line.size(); ++i) {
        if (line[i] == ':') {
            label_pos = i;
            break;
        }
    }
    string rem_line;
    if (label_pos != string::npos){
        line.erase(0, label_pos + 1);
        rem_line=line;
    }
    else{
        // Otherwise line without label is the line itself
        rem_line = line;
    }
    return rem_line;
}

// Assign Label with pc
map<string, int> label_pc;

// Check if the given string is a addable label
void is_valid_label(string s){
    int n = s.length();
    // Should not be in symbol table (already declared)
    if (label_pc.find(s) != label_pc.end()){
        string temp="ERROR: Line ";
        temp.append(to_string(PC));
        temp.append(": Label is already used.");
        errors.insert(temp);
        return;
    }

    // Should not be a mnemonic
    else if (mnemonic.find(s) != mnemonic.end()){
        string temp="ERROR: Line ";
        temp.append(to_string(PC));
        temp.append(": Label name is a reserved word.");
        errors.insert(temp);
        return;
    }

    // Should be alphanumeric
    for (int i=0; i<n; i++){
        if (!(is_small_alpha(s[i])|| is_capital_alpha(s[i]) || is_num(s[i]))){
            string temp="ERROR: Line ";
            temp.append(to_string(PC));
            temp.append(": Invalid label name found, It should be an alphanumberic string.");
            errors.insert(temp);
            return;
        }
    }

    // First letter is an alphabet
    if (is_num(s[0])){
        string temp="ERROR: Line ";
        temp.append(to_string(PC));
        temp.append(": Invalid label name found, It should begin with a letter.");
        errors.insert(temp);
    }
}

// Used to check if the given string is a possible number
bool is_valid_number(string s){
    int n = s.length();
    if ((n>=2)&&(s[0]=='0')&&!is_num(s[1])){
        // Binary
        if (s[1] == 'b'){
            int a = 2;
            for (int i=2; i<n; i++){
                if (!is_binary(s[i])){
                    a=0;
                    break;
                }
            }
            return a;
        }

        // Hexadecimal
        else if (s[1]=='x'){
            int a = 16;
            for (int i=2; i<n; i++){
                if (!(is_small_alpha(s[i])|| is_capital_alpha(s[i]) || is_num(s[i]))){
                    a=0;
                    break;
                }
            }
            return a;
        }

        //Octal
        else if (s[1]=='o'){
            int a = 8;
            for (int i=2; i<n; i++){
                if (!is_octal(s[i])){
                    a=0;
                    break;
                }
            }
            return a;
        }
        //Not a binary, hex or octal 
        else return 0;
    }
    else if ((n>=2)&&((s[0]=='+')||(s[0]=='-'))){
        // Decimal with sign
        int a = 10;
        for (int i=1; i<n; i++){
            if (!is_num(s[i])){
                a=0;
                break;
            }
        }
        return a;
    }
    else{
        // Decimal without sign
        int a = 10;
        for (int i=0; i<n; i++){
            if (!is_num(s[i])){
                a=0;
                break;
            }
        }
        return a;
    }
    return 0;
}

ll power(ll a, ll b){
    if(b==0) return 1;
    ll ans = power(a,b/2);
    if(b%2==0){
        return (ans*ans);
    }else{
        return ((ans*ans)*a);
    }
}

string hexToBinary(string hexString) {
    string binaryString = "";
    for (char hexChar : hexString) {
        int value;
        if (is_num(hexChar)) {
            value = hexChar - '0';
        } else if (is_capital_alpha(hexChar)) {
            value = hexChar - 'A' + 10;
        } else if (is_small_alpha(hexChar)) {
            value = hexChar - 'a' + 10;
        }
        binaryString += bitset<4>(value).to_string();
    }
    return binaryString;
}

string octToBinary(string octString) {
    string binaryString = "";
    for (char octChar : octString) {
        int value = octChar - '0';
        binaryString += bitset<3>(value).to_string();
    }
    return binaryString;
}

int BinaryToDecimal(string binarystr){
    int result = 0;
    reverse(binarystr.begin(),binarystr.end());
    int n = binarystr.length();
    if(n<=31){
        for (int i=0; i<min(n,31); i++) {
            if (binarystr[i] == '1') {
                result += power(2,i);
            }
        }
    }
    else{
        if(binarystr[31]=='1'){
            bool first1=false;
            for(int i=0; i<min(n,31); i++){
                if(first1){
                    if(binarystr[i]=='1') binarystr[i]='0';
                    else binarystr[i]='1';
                }
                if(!first1&&binarystr[i]=='1') first1=true;
            }
            for (int i=0; i<min(n,31); i++) {
                if (binarystr[i] == '1') {
                    result += power(2,i);
                }
            }
            result*=-1;
        }
        else{
            for (int i=0; i<min(n,31); i++) {
                if (binarystr[i] == '1') {
                    result += power(2,i);
                }
            }
        }
    }
    return result;
}

bool check_operand_0(string rem_line, int opcode, int oprand_no){
    stringstream ss(rem_line);
    string mnem;
    ss >> mnem;
    string nulstr;
    ss >> nulstr;
    if (nulstr != "")
    {
        string temp="ERROR: Line ";
        temp.append(to_string(PC));
        temp.append(": Unexpected operand.");
        errors.insert(temp);
        return false;
    }
    return true;
}

bool check_operand_1(string rem_line, int opcode, int oprand_no){
    stringstream ss(rem_line);
    string mnem;
    ss >> mnem;
    string valstr, nulstr;
    ss >> valstr;
    ss >> nulstr;
    if (valstr == ""){
        string temp="ERROR: Line ";
        temp.append(to_string(PC));
        temp.append(": Missing operand.");
        errors.insert(temp);
        return false;
    }

    if (nulstr != ""){
        string temp="ERROR: Line ";
        temp.append(to_string(PC));
        temp.append(": Extra operand.");
        errors.insert(temp);
        return false;
    }
    return true;
}

bool check_operand_2(string rem_line, int opcode, int oprand_no){
    stringstream ss(rem_line);
    string mnem;
    ss >> mnem;
    string offsetstr, nulstr;
    ss >> offsetstr;
    ss >> nulstr;

    if (offsetstr == ""){
        string temp="ERROR: Line ";
        temp.append(to_string(PC));
        temp.append(": Missing operand.");
        errors.insert(temp);
        return false;
    }

    if (nulstr != ""){
        string temp="ERROR: Line ";
        temp.append(to_string(PC));
        temp.append(": Extra operand.");
        errors.insert(temp);
        return false;
    }
    return true;
}

set<string> unused_labels;

void addtounused(string l){
    unused_labels.insert(l);
}

void remove_from_unused(string l){
    if (unused_labels.find(l) != unused_labels.end()){
        unused_labels.erase(l);
    }
}

// Used to convert given number string to integer
int convert_to_num(string s){
    int n = s.length();
    if(n>=2&&is_num(s[1])){
        int i=0;
        while(s[i]=='0') i++;
        s = s.substr(i,string::npos);
    }
    n = s.length();
    int num = 0;
    if ((n>=2)&&(s[0]=='0')&&!(is_num(s[1]))){
        // Binary
        if (s[1]=='b'){
            s = s.substr(2, string::npos);
            num = BinaryToDecimal(s);
        }
        // Hexadecimal
        else if (s[1]=='x'){
            s = s.substr(2, string::npos);
            s = hexToBinary(s);
            num = BinaryToDecimal(s);
            // stringstream numstr;
            // numstr << hex << s;
            // numstr >> num;
        }
        // Octal
        else{
            s = s.substr(1, string::npos);
            s = octToBinary(s);
            num = BinaryToDecimal(s);
            // stringstream numstr;
            // numstr << oct << s;
            // numstr >> num;
        }
    }
    else{
        // Decimal
        stringstream numstr;
        numstr << s;
        numstr >> num;
    }
    return num;
}

// relates mnemonic code to values taken as input
vector<int> opcode_input = {1, 1, 2, 2, 2, 2, 0, 0, 0, 0, 1, 0, 0, 2, 0, 2, 2, 2, 0, 1, 1};

// Used to store warnings
set<pair<int, string>> pc_Warn;

// assign data with pc
map<int, int> pc_data;

// backpatch list
map<int, string> backpatch_list;

// IMplement first pass
int first_pass(ifstream &asm_file){
    // If file open
    if (asm_file.is_open()){
        string line;
        while (getline(asm_file, line)){
            PC++;
            // Remove comments
            line = line.substr(0,line.find(";", 0));
            // If line empty
            if (whitespace_remover(line)==""){
                PC--;
                continue;
            }
            bool label_present = false;
            string label = givelable(line);

            // Checking for label
            size_t label_pos = line.find(":");

            if (label!=""){
                // Verifying if label is good, else adding error statemnts.
                label_present = true;
                is_valid_label(label);
                // Adding to symbol table
                label_pc[label] = PC;

                // Adding to unused labels
                addtounused(label);

                // Adding to listings
                Listings lst;
                lst.instruction = label;
                lst.program_counter = PC;
                lst.opcode = -1;
                lst.operand = 0;
                listings.push_back(lst);
            }

            // Used to store the line without label
            string rem_line = give_rem_line(line);

            if (whitespace_remover(rem_line) == ""){
                // Decrementing PC to preserve execution order. Since labels ae not present in final output code.
                PC--;
                continue;
            }

            stringstream ss(rem_line);
            string mnem;
            ss >> mnem;
            // Extracting mnemonic

            // Checking if mnemonic is there or not
            if (mnem==""){
                continue;
            }
            if (mnemonic.find(mnem)== mnemonic.end()){
                string temp="ERROR: Line ";
                temp.append(to_string(PC));
                temp.append(": Invalid mnemonic found.");
                errors.insert(temp);
                continue;
            }

            int opcode = mnemonic[mnem];
            int operand_no = opcode_input[opcode];
            if (opcode<19){
                // If no operand req
                if (operand_no == 0){
                    // string nulstr;
                    // ss >> nulstr;
                    // if (nulstr != ""){
                    //     errors.insert("ERROR: Line " + to_string(PC) + ": Unexpected operand.");
                    //     continue;
                    // }
                    if(check_operand_0){
                        Listings lst;
                        lst.instruction = line;
                        lst.opcode = opcode;
                        lst.operand = 0;
                        lst.program_counter = PC;
                        listings.push_back(lst);
                    }
                    else continue;
                }

                // If value req.
                else if (operand_no == 1){

                    // if (valstr == ""){
                    //     errors.insert("ERROR: Line " + to_string(PC) + ": Missing operand.");
                    //     continue;
                    // }

                    // if (nulstr != ""){
                    //     errors.insert("ERROR: Line " + to_string(PC) + ": Extra operand.");
                    //     continue;
                    // }
                    if(check_operand_1){
                        Listings lst;
                        lst.instruction = line;
                        lst.opcode = opcode;
                        lst.program_counter = PC;
                        string valstr, nulstr;
                        ss >> valstr;
                        ss >> nulstr;
                        // If number convert to number
                        if (is_valid_number(valstr)){
                            lst.operand = convert_to_num(valstr);
                        }
                        else if (is_possible_label(valstr)){
                            // If predefined label, replace with label name
                            if (label_pc.find(valstr) != label_pc.end()){
                                lst.operand = label_pc[valstr];
                                // If label represens data, replace with the data value
                                if (pc_data.find(label_pc[valstr]) != pc_data.end()){
                                    lst.operand = pc_data[label_pc[valstr]];
                                }

                                // Removing from unused labels
                                remove_from_unused(valstr);
                            }
                            else{
                                // Else append to backpatch list
                                lst.operand = 0;
                                lst.op_backpatch = true;
                                backpatch_list[PC] = valstr;
                            }
                        }
                        else if (is_num(valstr[0])){
                            string temp="ERROR: Line ";
                            temp.append(to_string(PC));
                            temp.append(": Not a number.");
                            errors.insert(temp);
                            continue;
                        }
                        else{
                            string temp="ERROR: Line ";
                            temp.append(to_string(PC));
                            temp.append(": Invalid operand.");
                            errors.insert(temp);
                            continue;
                        }
                        listings.push_back(lst);
                    }
                    else continue;
                }

                // If label has offset as operand
                else{
                    // string offsetstr, nulstr;
                    // ss >> offsetstr;
                    // ss >> nulstr;

                    // if (offsetstr == ""){
                    //     errors.insert("ERROR: Line " + to_string(PC) + ": Missing operand.");
                    //     continue;
                    // }

                    // if (nulstr != ""){
                    //     errors.insert("ERROR: Line " + to_string(PC) + ": Extra operand.");
                    //     continue;
                    // }
                    if(check_operand_2(rem_line,opcode,operand_no)){
                        Listings lst;
                        lst.instruction = line;
                        lst.opcode = opcode;
                        lst.program_counter = PC;
                        string offsetstr, nulstr;
                        ss >> offsetstr;
                        ss >> nulstr;
                        // If operand is number, set number as oprand
                        if (is_valid_number(offsetstr)){
                            lst.operand = convert_to_num(offsetstr);
                        }

                        // If possible label check
                        else if (is_possible_label(offsetstr)){
                            // If predefined label, set operand value
                            if (label_pc.find(offsetstr) != label_pc.end()){
                                lst.operand = label_pc[offsetstr] - PC - 1;
                                remove_from_unused(offsetstr);
                            }
                            else{
                                // Else append to backpatch list
                                lst.operand = 0;
                                lst.op_backpatch = true;
                                backpatch_list[PC] = offsetstr;
                            }
                        }

                        else if (is_num(offsetstr[0])){
                            string temp="ERROR: Line ";
                            temp.append(to_string(PC));
                            temp.append(": Not a number.");
                            errors.insert(temp);
                            continue;
                        }
                        else{
                            string temp="ERROR: Line ";
                            temp.append(to_string(PC));
                            temp.append(": Invalid operand.");
                            errors.insert(temp);
                            continue;
                        }

                        if ((lst.opcode == 17) && (lst.operand == -1)){
                            // Detecting infinite loops.
                            string temp="WARNING : Line ";
                            temp.append(to_string(PC));
                            temp.append(": Infinite loop detected.");
                            pc_Warn.insert({PC, temp});
                        }
                        listings.push_back(lst);
                    }
                    else continue;
                }
            }

            // Handling data
            else if (opcode==19){
                string valstr, nulstr;
                ss >> valstr;
                ss >> nulstr;

                if (valstr == ""){
                    string temp="ERROR: Line ";
                    temp.append(to_string(PC));
                    temp.append(": Missing operand.");
                    errors.insert(temp);
                    continue;
                }

                if (nulstr != ""){
                    string temp="ERROR: Line ";
                    temp.append(to_string(PC));
                    temp.append(": Extra operand.");
                    errors.insert(temp);
                    continue;
                }

                Listings lst;
                lst.instruction = line;
                lst.program_counter = PC;
                lst.opcode = 19;

                // Convert given value to number, if its a valid number
                if (is_valid_number(valstr)){
                    lst.operand = convert_to_num(valstr);
                }
                else{
                    string temp="ERROR: Line ";
                    temp.append(to_string(PC));
                    temp.append(": Not a number.");
                    errors.insert(temp);
                    continue;
                }

                // Add to data list
                pc_data[PC] = lst.operand;
                listings.push_back(lst);
            }
            // Handling set
            else{
                if (!label_present){
                    string temp="ERROR: Line ";
                    temp.append(to_string(PC));
                    temp.append(": Invalid use of SET without a label.");
                    errors.insert(temp);
                    continue;
                }

                listings.pop_back();

                string valstr, nulstr;

                ss >> valstr;
                ss >> nulstr;

                if (valstr == ""){
                    string temp="ERROR: Line ";
                    temp.append(to_string(PC));
                    temp.append(": Missing operand.");
                    errors.insert(temp);
                    continue;
                }

                if (nulstr != ""){
                    string temp="ERROR: Line ";
                    temp.append(to_string(PC));
                    temp.append(": Extra operand.");
                    errors.insert(temp);
                    continue;
                }

                Listings lst;
                lst.instruction = line;
                lst.program_counter = PC;
                lst.opcode = 20;

                if (is_valid_number(valstr)){
                    lst.operand = convert_to_num(valstr);
                }
                else{
                    string temp="ERROR: Line ";
                    temp.append(to_string(PC));
                    temp.append(": Not a number.");
                    errors.insert(temp);
                    continue;
                }

                label_pc[label] = lst.operand;

                listings.push_back(lst);
            }
        }
    }
    // If file not open.
    else{
        errors.insert("ERROR: File is empty.");
    }
    return 0;
}

// Used to store file instruction
vector<string> list_instruction;

// Store output integers
vector<unsigned int> instruction_int;

string binaryToHex(int num) {
    bitset<24> set(num);
    unsigned int ulong_num = set.to_ulong();
    stringstream ss;
    ss << std::hex << ulong_num;
    return ss.str();
}





int second_pass()
{
    for (auto it : listings)
    {
        // Convertin PC to hex
        array<char, 9> buffer;
        snprintf(buffer.data(), buffer.size(), "%08x", it.program_counter);
        string hexPC(buffer.data());

        // Except data, SET and labels
        if ((it.opcode < 19) && (it.opcode > -1)){
            if (it.op_backpatch){
                // item is in backpatch list
                string op_label = backpatch_list[it.program_counter];

                // if in symbol table
                if (label_pc.find(op_label) != label_pc.end()){
                    // value
                    if (opcode_input[it.opcode] == 1){
                        it.operand = label_pc[op_label];

                        // If some data value exists for the label
                        if (pc_data.find(label_pc[op_label]) != pc_data.end()){
                            it.operand = pc_data[label_pc[op_label]];
                        }
                    }
                    // offset
                    else if (opcode_input[it.opcode] == 2){
                        it.operand = label_pc[op_label]-(it.program_counter+1);
                    }

                    // remove from unused labels
                    remove_from_unused(op_label);
                }
                else{
                    string temp="ERROR: Line ";
                    temp.append(to_string(it.program_counter));
                    temp.append(": No such label.");
                    errors.insert(temp);
                }
            }

            stringstream sop;

            // converiting operand to hex
            sop << setfill('0') << setw(6) << hex<<binaryToHex(it.operand);

            // converting opcode to hex
            sop << setfill('0') << setw(2) << hex << it.opcode;

            string hexOP(sop.str());

            string temp=hexPC;
            temp+=' ';
            temp.append(hexOP);
            temp+=' ';
            temp.append(it.instruction);
            list_instruction.push_back(temp);

            // combining and pushing the integer into output

            stringstream ssd;
            ssd << hex << hexOP;
            unsigned int bin_op;
            ssd >> bin_op;
            instruction_int.push_back(bin_op);
        }
        else if (it.opcode == -1)
        {
            // if label, only add to listing file
            string temp=hexPC;
            temp+='\t';
            temp+='\t';
            temp+=' ';
            temp.append(it.instruction);
            temp+=':';
            list_instruction.push_back(temp);
        }
        else if (it.opcode == 19)
        {

            // If data, add as 32 bit integer
            stringstream sop;
            sop << setfill('0') << setw(8) << hex << it.operand;
            string hexOP(sop.str());
            
            string temp=hexPC;
            temp+=' ';
            temp.append(hexOP);
            temp+=' ';
            temp.append(it.instruction);
            list_instruction.push_back(temp);
            instruction_int.push_back(it.operand);
        }
        // if SET
        else
        {
            string temp=hexPC;
            temp+='\t';
            temp+='\t';
            temp+=' ';
            temp.append(it.instruction);
            list_instruction.push_back(temp);
        }
    }

    for (auto it : unused_labels)
    {
        string temp="WARNING : Line ";
        temp.append(to_string(label_pc[it]));
        temp.append(": Unused label - ");
        temp.append(it);
        pc_Warn.insert({label_pc[it], temp});
    }

    return 0;
}

bool check_valid_input(int cnt){
    if (!(cnt==2)){
        cout << "Given Input Format is Invalid. Valid Format is: ./asm asm_file.asm"<<endl;
        return true;
    }
    return false;
}

void make_file_error(string file_name, bool firstpass){
    string log_file_name = file_name;
    string listing_file_name = file_name;
    string object_file_name = file_name;
    log_file_name = log_file_name.append(".log"); //.log
    listing_file_name = listing_file_name.append(".l");
    object_file_name = object_file_name.append(".o");
    map<string, ofstream> files;
    files["log_file"].open(log_file_name, ios::out);
    files["listing_file"].open(listing_file_name, ios::out);
    files["object_file"].open(object_file_name, ios::out);
    //log_file.open(log_file_name, ios::out);
    int err=1;
    if(firstpass) files["log_file"] << "No Error Found in Pass 1." << endl;
    for (auto item=errors.begin(); item!=errors.end(); item++){
        files["log_file"] << err << ". " << *item << endl;
        err++;
    }
    // Empty files is errors in pass 1
    for (auto item=pc_Warn.begin(); item!=pc_Warn.end(); item++)
    {
        files["log_file"] << err << ". " << item->second << endl;
        err++;
    }
    files["listing_file_name"].close();
    files["object_file"].close();
    files["log_file"].close();
}

int main(int argc, char **argv){
    // Checking input format
    if(check_valid_input(argc)) return 0;

    string asm_file_name = string(argv[1]);
    string file_name = asm_file_name.substr(0, asm_file_name.find(".", 0));

    ifstream asm_file;
    asm_file.open(asm_file_name, ios::in);

    // first pass
    first_pass(asm_file);

    asm_file.close();

    if(errors.size()>0){
        make_file_error(file_name,false);
        return 0;
    }

    // second pass
    second_pass();

    if(errors.size()>0){
        make_file_error(file_name,true);
        return 0;
    }
    string log_file_name = file_name;
    string listing_file_name = file_name;
    string object_file_name = file_name;
    log_file_name = log_file_name.append(".log"); //.log
    listing_file_name = listing_file_name.append(".l");
    object_file_name = object_file_name.append(".o");
    map<string, ofstream> files;
    files["log_file"].open(log_file_name, ios::out);
    files["listing_file"].open(listing_file_name, ios::out);
    files["object_file"].open(object_file_name, ios::out | ios::binary);

    // printing errors
    int err = 1;
    files["log_file"] << "No Error Found in Pass 1." << endl;
    files["log_file"] << "No Error Found in Pass 2." << endl;

    // printing warnings
    for (auto item=pc_Warn.begin(); item!=pc_Warn.end(); item++){
        files["log_file"] << err << ". " << item->second << endl;
        err++;
    }

    files["log_file"].close();

    // listing file
    for (auto item=list_instruction.begin(); item!=list_instruction.end(); item++)
    {
        files["listing_file"] << *item << endl;
    }

    // output file
    for (auto item=instruction_int.begin(); item!=instruction_int.end(); item++)
    {
        int t = *item;
        files["object_file"].write((const char *)&t, sizeof(int));
    }

    files["listing_file"].close();
    files["object_file"].close();

    return 0;
}

