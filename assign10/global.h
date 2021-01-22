#pragma once

#include <unordered_map>
#include "Instruction.h"

extern float x; // probability of HIT
extern int N; // No of cycles needed in case of MISS

extern string input_instructions_path;
extern string clean_input_path;
extern string binary_input_path;

extern int reg_array[32];
extern string memory_array[4096];

extern vector<Instruction*> pipeline;

extern int pc;
extern int pc_branch;
extern bool use_branch_pc;

extern string reg_name[32];

extern int id;
extern int total_instr;

extern bool stalled;
extern int stalled_from;
extern int stall_cycles;
