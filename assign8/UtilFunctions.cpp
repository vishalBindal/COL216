#include "UtilFunctions.h"
#include "global.h"

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

int binary_to_int(string str, bool sign) {
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
    auto itr = find(reg_name, reg_name + 32, s);
    if (itr != end(reg_name))
        return binary_string(distance(reg_name, itr), 5);

    int len = s.length();

    if (s.at(0) != '$')
        return "";

    else
        return binary_string(stoi(s.substr(1)), 5);
}

string convert_instr(vector<string> tokens, unordered_map<string, int> labels, int mem_instr_index) {
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
            else
                return "";

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
        * Input: a string containing one line of a MIPS instruction
        * Converts inp into a vector of tokens using space, comma as delimiter
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
        } else if (c == ':') {
            if (intermediate.length() != 0)
                tokens.push_back(intermediate);
            intermediate.clear();
            tokens.push_back(":");
        } else
            intermediate.push_back(c);
    }

    if (intermediate.length() != 0)
        tokens.push_back(intermediate);

    return tokens;
}

void display_register_file() {
    /*
    Print the register file
    */
    cout << "\nRegister file: ";
    for (int regi : reg_array) {
        cout << regi << " ";
    }
    cout << endl;
}

bool search_for_label(vector<string> tokens) {
    /*
    Return whether a semicolon (indicating label presecence) present anywhere
    */
    int len = tokens.size();

    for (int i = 0; i < len; i++) {
        for (char j : tokens[i]) {
            if (j == ':')
                return true;
        }
    }

    return false;
}

string check_labels(string inp, unordered_map<string, int> *labels, int mem_instr_index) {
    /*
        * Check and remove any label or colon in inp, and return the remaining string
        * Update the map labels
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

void generate_clean_input_file() {
    /*
        Generate a 'clean' input file (with identical delimiters across all tokens, 
        and label in the same line as the corresponding instruction)
    */
    fstream infile, outfile;
    string inp;
    vector<string> parsed_inp;
    int k = 0;

    vector<vector<string>> tokens_file;
    vector<string> dummy_inp;
    bool found;

    infile.open(input_instructions_path, ios::in);

    while (getline(infile, inp)) {
        parsed_inp = parse_space(inp);

        if (!parsed_inp.empty()) {
            found = search_for_label(parsed_inp);

            if (found) {
                if (parsed_inp.size() == 2 && parsed_inp[1] == ":") {
                    getline(infile, inp);
                    if (infile) {
                        dummy_inp = parse_space(inp);
                        while (dummy_inp.empty()) {
                            getline(infile, inp);
                            if (!infile)
                                break;
                            dummy_inp = parse_space(inp);
                        }

                        for (const auto &i : dummy_inp)
                            parsed_inp.push_back(i);
                    }
                }
            }

            tokens_file.push_back(parsed_inp);
            k++;
        }
    }

    outfile.open(clean_input_path, ios::out);

    for (int i = 0; i < k; i++) {
        for (const auto &j : tokens_file[i])
            outfile << j << "   ";

        outfile << endl;
    }

    infile.close();
    outfile.close();
}

void display_empty(int stage) {
    switch (stage) {
        case 1:
            cout << "IF : ";
            break;
        case 2:
            cout << "ID : ";
            break;
        case 3:
            cout << "EX : ";
            break;
        case 4:
            cout << "MEM : ";
            break;
        case 5:
            cout << "WB : ";
    }
    cout << "---------------\n";
}

void print_binary_instructions_to_file() {
    string inp;
    fstream newfile;
    vector<string> input_without_labels;

    unordered_map<string, int> labels;
    newfile.open(clean_input_path, ios::in);
    int k = 0;
    if (newfile.is_open())
        while (getline(newfile, inp)) {
            input_without_labels.push_back(check_labels(inp, &labels, k));
            k++;
        }
    else {
        cerr << "Unable to open input file!" << endl;
        return;
    }
    newfile.close();

    fstream outfile;
    outfile.open(binary_input_path, ios::out);

    k = 0;
    for (const string &line: input_without_labels) {
        string instruct = convert_instr(parse_space(line), labels, k);
        if (!instruct.empty())
        {
            ++k;
            outfile << instruct << endl;
        }
    }
    outfile.close();
}

void fill_instructions_in_memory() {
    string line;
    fstream binary_input;
    binary_input.open(binary_input_path, ios::in);
    int k = 0;
    if (binary_input.is_open())
        while (getline(binary_input, line) && !line.empty())
            memory_array[k++] = line;
    else
        cerr << "Input file with binary instructions does not exist!\n";
    binary_input.close();
}
