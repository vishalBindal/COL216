#pragma once
#include <bits/stdc++.h>
using namespace std;

string binary_string(int a, int n);
/*
    * Input: an int a and the length required: n
    * Returns a string of len n containing binary representation of a
*/
int binary_to_int(string str, bool sign = false); 
/*
    * Input: string str containing binary number, bool sign denoting whether signed number
    * Returns int a equal to the value of str
*/
string reg_index(string s);
/*
    * Input : string containing register no as $no, eg. $5
    * Returns string of length 5 containing register no in binary eg. 00101
*/
string convert_instr(vector<string> tokens, unordered_map<string, int> labels, int mem_instr_index);
/*
    * Input: a vector of tokens of the mips instruction, eg {"add", "$1", "$2", "$3"}
    * returns a 32-char machine instruction
*/
vector<string> parse_space(string inp);
/*
    * Input: a string containing one line of a MIPS instruction
    * Converts inp into a vector of tokens using space, comma as delimiter
*/
void display_register_file();
/*
    Print the register file
*/
bool search_for_label(vector<string> tokens);
/*
    Return whether a semicolon (indicating label presecence) present anywhere
*/
string check_labels(string inp, unordered_map<string, int> *labels, int mem_instr_index);
/*
    * Check and remove any label or colon in inp, and return the remaining string
    * Update the map labels
*/
void generate_clean_input_file();
/*
    Generate a 'clean' input file (with identical delimiters across all tokens, 
    and label in the same line as the corresponding instruction)
*/
void display_empty(int stage);

void fill_instructions_in_memory();

void print_binary_instructions_to_file();