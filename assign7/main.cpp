#include <bits/stdc++.h>

using namespace std;

int reg_array[32];
string memory_array[4096];
int pc = 1;
bool debug = false;

string reg_name[32] = {"$0", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", "$t2", "$t3"
        , "$t4", "$t5", "$t6", "$t7", "$t8", "$t9", "$s0", "$s1", "$s2", "$s3"
        , "$s4", "$s5", "$s6", "$s7", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"};

map<string, int> delay;
int total_cycles = 0;
int total_instr = 0;

string binary_string(int a, int n) {
    /*
     * Input: an int a and the length required: n
     * Returns a string of len n containing binary representation of a
     */
    string str;
    string dum;
    int k = 0;
    bool neg = false;

    if (a < 0) {
        a *= (-1);
        neg = true;
    }

    while (a > 0) {
        str += '0' + a % 2;
        a = a / 2;
        k++;
    }

    while (k != n) {
        str += '0';
        k++;
    }

    if (neg)
        str.replace(n - 1, 1, "1");

    reverse(str.begin(), str.end());
    return str;
}

int binary_to_int(string str, bool sign = false) {
    /*
     * Input: string str containing binary no
     * Returns int a equal to the value of str
     */
    int len = (int) str.length() - 1;
    int a = 0;

    for (int i = 0; i < len; i++) {
        if (str.at(len - i) == '1')
            a += (int) pow(2, i);
    }
    if (sign) {
        if (str[0] == '1') a *= -1;
    } else {
        if (str[0] == '1')
            a += (int) pow(2, len);
    }
    return a;
}

string reg_index(string s) {
    /*
     * Input : string containing register no as $no, eg. $5
     * Returns string of length 5 containing register no in binary eg. 00101
     */
    auto itr = find(reg_name, reg_name+32, s);
    if( itr != end(reg_name))
        return binary_string(distance(reg_name, itr), 5);

    int len = s.length();

    if (s.at(0) != '$')
        return "";

    else
        return binary_string(stoi(s.substr(1)), 5);
}

bool exec_instr(string inp) {
    /*
     * Input: 32-char instruction inp
     * Executes instruction inp
     */
    int dest, src1, src2, temp;
    string opcode, funct;

    opcode = inp.substr(0, 6);
    funct = inp.substr(26, 6);
    if (opcode == "000000" && funct == "100000") {
        src1 = binary_to_int(inp.substr(6, 5));
        src2 = binary_to_int(inp.substr(11, 5));
        dest = binary_to_int(inp.substr(16, 5));
        reg_array[dest] = reg_array[src1] + reg_array[src2];
        total_instr++;
        total_cycles+=delay.find("add")->second;
        return true;
    } else if (opcode == "000000" && funct == "100010") {
        src1 = binary_to_int(inp.substr(6, 5));
        src2 = binary_to_int(inp.substr(11, 5));
        dest = binary_to_int(inp.substr(16, 5));

        reg_array[dest] = reg_array[src1] - reg_array[src2];
        total_instr++;
        total_cycles+=delay.find("sub")->second;
        return true;
    } else if (opcode == "000000" && funct == "000000") {
        src2 = binary_to_int(inp.substr(11, 5));
        dest = binary_to_int(inp.substr(16, 5));
        temp = binary_to_int(inp.substr(21, 5));

        reg_array[dest] = reg_array[src2] << temp;
        total_instr++;
        total_cycles+=delay.find("sll")->second;
        return true;
    } else if (opcode == "000000" && funct == "000010") {
        src2 = binary_to_int(inp.substr(11, 5));
        dest = binary_to_int(inp.substr(16, 5));
        temp = binary_to_int(inp.substr(21, 5));

        reg_array[dest] = reg_array[src2] >> temp;
        total_instr++;
        total_cycles+=delay.find("srl")->second;
        return true;
    } else if (opcode == "101011") {
        src2 = binary_to_int(inp.substr(6, 5));
        src1 = binary_to_int(inp.substr(11, 5));
        temp = binary_to_int(inp.substr(16, 16));
        int mem_pos = temp + reg_array[src2];
        memory_array[mem_pos] = binary_string(reg_array[src1], 32);
        cout << "--M-- Memory position " << mem_pos << " set to " << memory_array[mem_pos] << endl;
        total_instr++;
        total_cycles+=delay.find("sw")->second;
        return true;
    } else if (opcode == "100011") {
        src2 = binary_to_int(inp.substr(6, 5));
        src1 = binary_to_int(inp.substr(11, 5));
        temp = binary_to_int(inp.substr(16, 16));
        reg_array[src1] = binary_to_int(memory_array[temp + reg_array[src2]], true);
        total_instr++;
        total_cycles+=delay.find("lw")->second;
        return true;
    } else if (opcode == "000101") {
        src1 = binary_to_int(inp.substr(6, 5));
        src2 = binary_to_int(inp.substr(11, 5));
        temp = binary_to_int(inp.substr(16, 16));
        if (reg_array[src1] != reg_array[src2])
            pc += temp - 1;
        total_instr++;
        total_cycles+=delay.find("bne")->second;
        return true;
    } else if (opcode == "000100") {
        src1 = binary_to_int(inp.substr(6, 5));
        src2 = binary_to_int(inp.substr(11, 5));
        temp = binary_to_int(inp.substr(16, 16));
        if (reg_array[src1] == reg_array[src2])
            pc += temp - 1;
        total_instr++;
        total_cycles+=delay.find("beq")->second;
        return true;
    } else if (opcode == "000110") {
        src1 = binary_to_int(inp.substr(6, 5));
        temp = binary_to_int(inp.substr(16, 16));
        if (reg_array[src1] <= 0)
            pc += temp - 1;
        total_instr++;
        total_cycles+=delay.find("blez")->second;
        return true;
    } else if (opcode == "000111") {
        src1 = binary_to_int(inp.substr(6, 5));
        temp = binary_to_int(inp.substr(16, 16));
        if (reg_array[src1] > 0)
            pc += temp - 1;
        total_instr++;
        total_cycles+=delay.find("bgtz")->second;
        return true;
    } else if (opcode == "000010") {
        temp = binary_to_int(inp.substr(6, 26));
        pc = temp;
        total_instr++;
        total_cycles+=delay.find("j")->second;
        return true;
    } else if (opcode == "000011") {
        reg_array[31] = pc;
        temp = binary_to_int(inp.substr(6, 26));
        pc = temp;
        total_instr++;
        total_cycles+=delay.find("jal")->second;
        return true;
    } else if (opcode == "000000" && funct == "001000") {
        src1 = binary_to_int(inp.substr(6, 5));
        pc = reg_array[src1];
        total_instr++;
        total_cycles+=delay.find("jr")->second;
        return true;
    }

    return false;
}

string convert_instr(vector<string> tokens, map<string, int> labels, int mem_instr_index) {
    /*
     * Input: a vector of tokens of the mips instruction, eg {"add", "$1", "$2", "$3"}
     * returns a 32-char machine instruction
     */
    string dest, src1, src2, temp;
    string dummy;
    int len = tokens.size();

    if (len == 4) {

        if (tokens[0] == "add") {
            dest = reg_index(tokens[1]);
            src1 = reg_index(tokens[2]);
            src2 = reg_index(tokens[3]);
            if (dest.length() == 0 || src1.length() == 0 || src2.length() == 0)
                return "";

            dummy.append("000000");
            dummy.append(src1);
            dummy.append(src2);
            dummy.append(dest);
            dummy.append("00000");
            dummy.append("100000");

            return dummy;
        } else if (tokens[0] == "sub") {
            dest = reg_index(tokens[1]);
            src1 = reg_index(tokens[2]);
            src2 = reg_index(tokens[3]);

            if (dest.length() == 0 || src1.length() == 0 || src2.length() == 0)
                return "";

            dummy.append("000000");
            dummy.append(src1);
            dummy.append(src2);
            dummy.append(dest);
            dummy.append("00000");
            dummy.append("100010");

            return dummy;
        } else if (tokens[0] == "sll") {
            dest = reg_index(tokens[1]);
            src1 = reg_index(tokens[2]);
            temp = binary_string(stoi(tokens[3]), 5);

            if (dest.length() == 0 || src1.length() == 0)
                return "";

            dummy.append("000000");
            dummy.append("00000");
            dummy.append(src1);
            dummy.append(dest);
            dummy.append(temp);
            dummy.append("000000");

            return dummy;
        } else if (tokens[0] == "srl") {
            dest = reg_index(tokens[1]);
            src1 = reg_index(tokens[2]);
            temp = binary_string(stoi(tokens[3]), 5);

            if (dest.length() == 0 || src1.length() == 0)
                return "";

            dummy.append("000000");
            dummy.append("00000");
            dummy.append(src1);
            dummy.append(dest);
            dummy.append(temp);
            dummy.append("000010");

            return dummy;
        } else if (tokens[0] == "sw") {//sw $1 1024($2)
            src1 = reg_index(tokens[1]);
            temp = binary_string(stoi(tokens[2]), 16);
            src2 = reg_index(tokens[3]);

            if (src2.length() == 0 || src1.length() == 0)
                return "";

            dummy.append("101011");
            dummy.append(src2);
            dummy.append(src1);
            dummy.append(temp);

            return dummy;
        } else if (tokens[0] == "lw") {//lw $1 1024($2)
            src1 = reg_index(tokens[1]);
            temp = binary_string(stoi(tokens[2]), 16);
            src2 = reg_index(tokens[3]);

            if (src2.length() == 0 || src1.length() == 0)
                return "";

            dummy.append("100011");
            dummy.append(src2);
            dummy.append(src1);
            dummy.append(temp);

            return dummy;
        } else if (tokens[0] == "bne") {
            src1 = reg_index(tokens[1]);
            src2 = reg_index(tokens[2]);
            if (src2.length() == 0 || src1.length() == 0)
                return "";

            if (labels.find(tokens[3]) != labels.end())
                temp = binary_string(labels.find(tokens[3])->second - mem_instr_index, 16);
            else if (all_of(tokens[3].begin(), tokens[3].end(), ::isdigit))
                temp = binary_string(stoi(tokens[3]), 16);
            else return "";
            if (temp[0] == '1') return ""; //negative

            dummy.append("000101");
            dummy.append(src1);
            dummy.append(src2);
            dummy.append(temp);

            return dummy;
        } else if (tokens[0] == "beq") {
            src1 = reg_index(tokens[1]);
            src2 = reg_index(tokens[2]);
            if (src2.length() == 0 || src1.length() == 0)
                return "";

            if (labels.find(tokens[3]) != labels.end())
                temp = binary_string(labels.find(tokens[3])->second - mem_instr_index, 16);
            else if (all_of(tokens[3].begin(), tokens[3].end(), ::isdigit))
                temp = binary_string(stoi(tokens[3]), 16);
            else return "";
            if (temp[0] == '1') return "";

            dummy.append("000100");
            dummy.append(src1);
            dummy.append(src2);
            dummy.append(temp);

            return dummy;
        } else return "";

    } else if (len == 3) {
        if (tokens[0] == "blez") {
            src1 = reg_index(tokens[1]);
            if (src1.length() == 0)
                return "";

            if (labels.find(tokens[2]) != labels.end())
                temp = binary_string(labels.find(tokens[2])->second - mem_instr_index, 16);
            else if (all_of(tokens[2].begin(), tokens[2].end(), ::isdigit))
                temp = binary_string(stoi(tokens[2]), 16);
            else return "";
            if (temp[0] == '1') return "";

            dummy.append("000110");
            dummy.append(src1);
            dummy.append("00000");
            dummy.append(temp);

            return dummy;
        } else if (tokens[0] == "bgtz") {
            src1 = reg_index(tokens[1]);
            if (src1.length() == 0)
                return "";

            if (labels.find(tokens[2]) != labels.end())
                temp = binary_string(labels.find(tokens[2])->second - mem_instr_index, 16);
            else if (all_of(tokens[2].begin(), tokens[2].end(), ::isdigit))
                temp = binary_string(stoi(tokens[2]), 16);
            else return "";
            if (temp[0] == '1') return "";

            dummy.append("000111");
            dummy.append(src1);
            dummy.append("00000");
            dummy.append(temp);

            return dummy;
        } else return "";
    } else if (len == 2) {
        if (tokens[0] == "j") {
            if (labels.find(tokens[1]) != labels.end())
                temp = binary_string(labels.find(tokens[1])->second, 26);
            else if (all_of(tokens[1].begin(), tokens[1].end(), ::isdigit))
                temp = binary_string(stoi(tokens[1]), 26);
            else return "";

            dummy.append("000010");
            dummy.append(temp);

            return dummy;
        } else if (tokens[0] == "jal") {
            if (labels.find(tokens[1]) != labels.end())
                temp = binary_string(labels.find(tokens[1])->second, 26);
            else if (all_of(tokens[1].begin(), tokens[1].end(), ::isdigit))
                temp = binary_string(stoi(tokens[1]), 26);
            else return "";

            dummy.append("000011");
            dummy.append(temp);

            return dummy;
        } else if (tokens[0] == "jr") {
            src1 = reg_index(tokens[1]);
            if (src1.length() == 0)
                return "";
            dummy.append("000000");
            dummy.append(src1);
            dummy.append("000000000000000001000");
            return dummy;
        } else return "";
    } else return "";
}

vector<string> parse_space(string inp) {
    /*
     * Convert inp (mips instruction) into a vector of tokens using space, comma as delimiter
     */
    vector<string> tokens;
    int len = inp.length();
    string intermediate;
    char c;

    for (int i = 0; i < len; i++) {
        c = inp.at(i);
        if (c == ' ' || c == '(' || c == ')' || c == ',' || c == '\t') {
            if (intermediate.length() != 0)
                tokens.push_back(intermediate);
            intermediate.clear();
        }
        else if(c==':'){
            if (intermediate.length() != 0)
                tokens.push_back(intermediate);
            intermediate.clear();
            tokens.push_back(":");
        }else
            intermediate.push_back(c);
    }

    if (intermediate.length() != 0)
        tokens.push_back(intermediate);

    return tokens;
}

void display() {
    cout << "--R-- Register file: ";
    for (int regi : reg_array) {
        cout << regi << " ";
    }
    cout << endl;
}

bool search_for_label(vector<string> tokens) {
    int len = tokens.size();

    for (int i = 0; i < len; i++) {
        for (int j = 0; j < tokens[i].length(); j++) {
            if (tokens[i].at(j) == ':')
                return true;
        }
    }

    return false;
}

string check_labels(string inp, map<string, int> *labels, int mem_instr_index) {
    /*
     * Checks for possible labels in line
     * also removes the label and colon, and returns the remaining string
     */
    int pos = inp.find(':');
    while (pos != string::npos) {
        int endpos = pos - 1;
        if (endpos == -1) return inp;
        while (endpos >= 0 && (inp[endpos] == ' ' || inp[endpos] == '\t')) endpos--;
        int startpos = endpos;
        while (startpos > 0 && inp[startpos - 1] != ' ' && inp[startpos - 1] != '\t' &&
               inp[startpos - 1] != ',')
            startpos--;
        labels->insert({inp.substr(startpos, endpos - startpos + 1), mem_instr_index});
        inp.erase(startpos, pos - startpos + 1);

        pos = inp.find(':');
    }
    return inp;
}

int main() {
    fstream cyclefile;
    vector<string> parsed_inp;
    string inp;
    cyclefile.open("delay.txt");
    while(getline(cyclefile, inp))
    {
        parsed_inp = parse_space(inp);
        if(parsed_inp.size()==2)
            delay.insert({parsed_inp[0], stoi(parsed_inp[1])});
    }
    cyclefile.close();

    fstream infile, outfile;
    int k = 0;

    vector<vector<string>> tokens_file;
    vector<string> dummy_inp;
    bool found;

    infile.open("input.txt", ios::in);

    while (getline(infile, inp)) {
        parsed_inp = parse_space(inp);

        if (parsed_inp.size() != 0) {
            found = search_for_label(parsed_inp);

            if (found) {
                if(parsed_inp.size()==2 && parsed_inp[1]==":") {
                    getline(infile, inp);
                    if (infile) {
                        dummy_inp = parse_space(inp);
                        while (dummy_inp.size() == 0) {
                            getline(infile, inp);
                            if (!infile)
                                break;
                            dummy_inp = parse_space(inp);
                        }

                        for (int i = 0; i < dummy_inp.size(); i++)
                            parsed_inp.push_back(dummy_inp[i]);
                    }
                }
            }

            tokens_file.push_back(parsed_inp);
            k++;
        }
    }

    outfile.open("./input_clean.txt", ios::out);

    for (int i = 0; i < k; i++) {
        for (int j = 0; j < tokens_file[i].size(); j++)
            outfile << tokens_file[i][j] << "   ";

        outfile << endl;
    }

    infile.close();
    outfile.close();
    fstream newfile;
    bool done;
    vector<string> input_without_labels;

    for (int &regi : reg_array)
        regi = 0;
    reg_array[0] = 2;
    reg_array[1] = 1;
    reg_array[29] = 4095;
    display();

    map<string, int> labels;
    newfile.open("./input_clean.txt", ios::in);
    k = 0;
    if (newfile.is_open())
        while (getline(newfile, inp)) {
            input_without_labels.push_back(check_labels(inp, &labels, k));
            k++;
        }
    else {
        cout << "Unable to open input file!" << endl;
        return 1;
    }
    newfile.close();

    k = 0;
    for (const string &line: input_without_labels) {
        string instruct = convert_instr(parse_space(line), labels, k);
        if (!instruct.empty())
            memory_array[k++] = instruct;
    }

    while (pc <= 4096 && !memory_array[pc - 1].empty()) {
        if(debug) getchar();
        inp = memory_array[pc - 1];
        cout << "pc = " << pc << ", instruction: " << inp << endl;
        done = exec_instr(inp);
        display();
        if (!done) {
            cout << "Breaking\n";
            break;
        }
        pc++;
    }
    cout<<endl<<"Total cycles: "<<total_cycles<<endl<<
    "Average instructions per cycle (IPC) : "<<(float)total_instr/total_cycles<<endl;
    return 0;
}
