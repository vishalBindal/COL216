#include "Main_memory.h"

Main_memory* mainMemory;

Main_memory::Main_memory(int mem_size, int block_size) {
    memory = vector<vector<bool>>(mem_size, vector<bool>(block_size*8, false));
}

vector<bool> Main_memory::read(vector<bool> &address)
/*
 * Return block of data which the given byte address is part of
 */
{
    int addr = (int)binary_to_int(address);
    if (addr >= 0 && addr < memory.size())
    {
        vector<bool> data = memory[addr];
        if(show_details)
            cout<<"-->Read "<<binary_to_int(data,false)<<" from memory address "<<addr<<endl;
        return data;
    }
    else
    {
        cout << "Main Memory address "<<addr<<" out of bounds\n";
        return vector<bool>();
    }
}

bool Main_memory::write(vector<bool> &address, vector<bool> &data)
{
    int addr = (int)binary_to_int(address);
    if(addr >= 0 && addr < memory.size())
    {
        memory[addr] = data;
        if(show_details)
            cout<<"-->Written "<<binary_to_int(data,false)<<" to memory address "<<addr<<endl;
        return true;
    }
    else {
        cout << "Main Memory address " << addr << " out of bounds\n";
        return false;
    }
}
