#include "global.h"

float x = 0.2;
int N = 10;

int reg_array[32];
string memory_array[4096];
int pc = 1;

vector<Instruction*> pipeline;

string input_instructions_path = "./input.txt";
string clean_input_path = "./input_clean.txt";
string binary_input_path = "./binary_input.txt";
string reg_name[32] = {"$0", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", "$t2", "$t3"
        , "$t4", "$t5", "$t6", "$t7", "$t8", "$t9", "$s0", "$s1", "$s2", "$s3"
        , "$s4", "$s5", "$s6", "$s7", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"};

int id = 0;
int total_instr = 0;

bool stalled = false;
int stalled_from = 0;
int stall_cycles = 0;

int pc_branch = 1;
bool use_branch_pc = false;
