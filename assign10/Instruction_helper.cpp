#include "Instruction_helper.h"

bool instruction_fetch() {
    if (use_branch_pc) {
        pc = pc_branch + 1;
        use_branch_pc = false;
    }

    if (pc > 4096 || pc < 1 || memory_array[pc - 1].empty())
        return false;

    string instruction = memory_array[pc - 1];

    auto instr = new Instruction(instruction, pc, id);
    ++id;
    pipeline.push_back(instr);

    cout << "IF : Instruction " << pc << endl;

    ++pc;
    return true;
}

void instruction_perform(int &i, int &pipe_size) {
    // i is index of instruction in pipeline
    bool isStalled = false;
    if (stalled && pipeline[i]->id >= stalled_from)
        isStalled = true;
    switch (pipeline[i]->stage) {
        case 2:
            cout << "ID : Instruction " << pipeline[i]->pc_instr;
            if (!isStalled) pipeline[i]->instruction_decode();
            break;
        case 3:
            cout << "EX : Instruction " << pipeline[i]->pc_instr;
            if (!isStalled) pipeline[i]->execute();
            break;

        case 4:
            cout << "MEM : Instruction " << pipeline[i]->pc_instr;
            if (!isStalled) pipeline[i]->memory_access();
            break;

        case 5:
            cout << "WB : Instruction " << pipeline[i]->pc_instr;
            if (!isStalled) pipeline[i]->write_back();
            i--; // to ensure the condition of i index remains since we erased one element
            pipe_size--;
            break;

        default:
            break;
    }
    if (isStalled)
        cout << " <STALLED>";
    cout << endl;
}