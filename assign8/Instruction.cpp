#include "Instruction.h"
#include "UtilFunctions.h"
#include "global.h"
#include <random>

Instruction::Instruction(string instr, int instr_pc, int id) {
    this->instr = instr;
    this->pc_instr = instr_pc;
    this->id = id;
    this->stage = 2;
}

void stall(int instr_id, int n)
// stall instructions from i onwards in the pipeline for n cycles
{
    stalled = true;
    stalled_from = instr_id;
    stall_cycles = n;
}

bool getData(int reg_index, int &data)
/*
 * Input: Reg Index at which data needed
 * Returns if data retrieval successful and sets data to the required value,
 */
{
    for (int i=(int)pipeline.size()-2; i>=0; --i) 
        if(pipeline[i]->is_data_hazard(reg_index))
            return false;

    data = reg_array[reg_index];
    return true;
}

void Instruction::instruction_decode() {

    string opcode = instr.substr(0, 6);
    string funct = instr.substr(26, 6);

    if (opcode == "000000" && funct == "100000")
        type = 0; //add
    else if (opcode == "000000" && funct == "100010")
        type = 1; //sub
    else if (opcode == "000000" && funct == "000000")
        type = 2; //sll
    else if (opcode == "000000" && funct == "000010")
        type = 3; //srl
    else if (opcode == "101011")
        type = 4; //sw
    else if (opcode == "100011")
        type = 5; //lw
    else if (opcode == "000101")
        type = 6; //bne
    else if (opcode == "000100")
        type = 7; //beq
    else if (opcode == "000110")
        type = 8; //blez
    else if (opcode == "000111")
        type = 9; //bgtz
    else if (opcode == "000010")
        type = 10; //j
    else if (opcode == "000011")
        type = 11; //jal
    else if (opcode == "000000" && funct == "001000")
        type = 12; //jr

    switch (type) {

        case 0:
        case 1: {
            rs_reg_index = binary_to_int(instr.substr(6, 5));
            rt_reg_index = binary_to_int(instr.substr(11, 5));
            rd_reg_index = binary_to_int(instr.substr(16, 5));
            if (getData(rs_reg_index, rs_value)
                && getData(rt_reg_index, rt_value)) {
                ++stage;
            } else
                stall(id, 1);

            break;
        }

        case 2:
        case 3:
            rt_reg_index = binary_to_int(instr.substr(11, 5));
            rd_reg_index = binary_to_int(instr.substr(16, 5));
            offset = binary_to_int(instr.substr(21, 5));
            if (getData(rt_reg_index, rt_value)) {
                ++stage;
            } else
                stall(id, 1);
            break;

        case 4: {
            rs_reg_index = binary_to_int(instr.substr(6, 5));
            rt_reg_index = binary_to_int(instr.substr(11, 5));
            offset = binary_to_int(instr.substr(16, 16));
            if (getData(rs_reg_index, rs_value) &&
                getData(rt_reg_index, rt_value)) {
                data_32bit = binary_string(rt_value, 32);
                ++stage;
            } else
                stall(id, 1);
            break;
        }

        case 5:
            rs_reg_index = binary_to_int(instr.substr(6, 5));
            rt_reg_index = binary_to_int(instr.substr(11, 5));
            offset = binary_to_int(instr.substr(16, 16));
            if (getData(rs_reg_index, rs_value)) {
                ++stage;
            } else
                stall(id, 1);

            break;

        case 6:
        case 7: {
            rs_reg_index = binary_to_int(instr.substr(6, 5));
            rt_reg_index = binary_to_int(instr.substr(11, 5));
            offset = binary_to_int(instr.substr(16, 16), true);
            if (getData(rs_reg_index, rs_value) &&
                getData(rt_reg_index, rt_value)) {
                ++stage;
                stall(id + 1, 2);
            } else
                stall(id, 1);
            break;
        }

        case 8:
        case 9:
            rs_reg_index = binary_to_int(instr.substr(6, 5));
            offset = binary_to_int(instr.substr(16, 16), true);
            if (getData(rs_reg_index, rs_value)) {
                ++stage;
                stall(id + 1, 2);
            } else
                stall(id, 1);

            break;

        case 10:
            pc_branch = binary_to_int(instr.substr(6, 26));
            use_branch_pc = true;
            stage++;
            stall(id + 1, 1);
            break;

        case 11:
            pc_branch = binary_to_int(instr.substr(6, 26));
            use_branch_pc = true;
            stage++;
            stall(id + 1, 1);
            break;

        case 12:
            rs_reg_index = binary_to_int(instr.substr(6, 5));
            if (getData(rs_reg_index, pc_branch)) {
                use_branch_pc = true;
                ++stage;
                stall(id + 1, 1);
            } else
                stall(id, 1);
            break;
    }
}

void Instruction::execute() {

    switch (type) {
        case 0:
            result = rs_value + rt_value;
            break;

        case 1:
            result = rs_value - rt_value;
            break;

        case 2:
            result = rt_value << offset;
            break;

        case 3:
            result = rt_value >> offset;
            break;

        case 4:
            result = rs_value + offset;
            break;

        case 5:
            result = rs_value + offset;
            break;

        case 6:
            if (rs_value != rt_value) {
                pc_branch = pc_instr + offset - 1;
                use_branch_pc = true;
            }
            break;

        case 7:
            if (rs_value == rt_value) {
                pc_branch = pc_instr + offset - 1;
                use_branch_pc = true;
            }
            break;

        case 8:
            if (rs_value <= 0) {
                pc_branch = pc_instr + offset - 1;
                use_branch_pc = true;
            }
            break;

        case 9:
            if (rs_value > 0) {
                pc_branch = pc_instr + offset - 1;
                use_branch_pc = true;
            }
            break;

        default:
            break;
    }
    ++stage;
}

void Instruction::memory_access() {

    switch (type) {

        case 4: //sw
            memory_array[result] = data_32bit;
            cout << "\n\tWrite to memory " << result << " := " << data_32bit;
            break;

        case 5: //lw
            data_32bit = memory_array[result];
            cout << "\n\tRead from memory " << result << " := " << data_32bit;
            break;

        default:
            break;
    }
    ++stage;
}

void Instruction::write_back() {
    total_instr++; // each instr written back means it has been completely executed

    switch (type) {

        case 0:
        case 1:
        case 2:
        case 3:
            reg_array[rd_reg_index] = result;
            cout << "\n\tWrite to register index " << rd_reg_index << " := " << result;
            pipeline.erase(pipeline.begin());
            break;

        case 5:
            reg_array[rt_reg_index] = binary_to_int(data_32bit, true);
            cout << "\n\tWrite to register index " << rt_reg_index << " := " << binary_to_int(data_32bit, true);
            pipeline.erase(pipeline.begin());
            break;
        case 11:
            reg_array[31] = pc_instr;
            cout << "\n\tWrite to register index 31 := " << pc_instr;
            pipeline.erase(pipeline.begin());
            break;
        default:
            pipeline.erase(pipeline.begin());
            break;
    }
}

bool Instruction::is_data_hazard(int reg_index)
{
    switch (type) {
        case 0:
        case 1:
        case 2:
        case 3:
            return (rd_reg_index == reg_index);
        case 5:
            return (rt_reg_index == reg_index);
        case 11:
            return (reg_index == 31);
        default:
            return false;
    }
}