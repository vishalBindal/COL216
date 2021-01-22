#pragma once

#include "Line.h"
#include "Main_memory.h"

using namespace std;

class Set {
private:
    vector<Line> lines;
    int a, T;
    vector<bool> set_index;
    int get_appropriate_pos();

public:
    Set();
    Set(int a, int T, int tag_size_bits, int data_size_bits, vector<bool> set_index);
    bool write(vector<bool> &tag, vector<bool> &data, bool &is_hit);
    vector<bool> read(vector<bool> &tag, vector<bool> &address, bool &is_hit);
    void update_groups();
    void print_set();
};
