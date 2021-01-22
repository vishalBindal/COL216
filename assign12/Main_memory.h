#pragma once

#include "general.h"

class Main_memory {
private:
    vector<vector<bool>> memory; // array of blocks

public:
    Main_memory(int mem_size, int block_size);
    vector<bool> read(vector<bool> &address);
    bool write(vector<bool> &address, vector<bool> &data);
};

extern Main_memory* mainMemory;
