#pragma once

#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Instruction{
private:
    int rs_reg_index, rt_reg_index, rd_reg_index;
    int offset;

    int rs_value, rt_value;
    int result;

    string data_32bit;
    bool ongoing_mem_access;

public:
    string instr;
    int id;
    int pc_instr;
    int stage;
    //indicates stage of execution
    //2 : ID, 3 : EX, 4 : MEM, 5 : WB
    int type;
    // 0 : add , 1 : sub , 2 : sll , 3 : srl , 4 : sw , 5 : lw, 6 : bne
    // 7 : beq , 8 : blez , 9 : bgtz , 10 : j , 11 : jal, 12 : jr
    Instruction(string instr, int instr_pc, int id);
    void instruction_decode();
    void execute();
    void memory_access();
    void write_back();
    bool is_data_hazard(int reg_index);
    bool okay_to_forward(int reg_index);
    //returns true if information can be forwarded for data at reg_index
    int forward_data();
};