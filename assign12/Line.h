#pragma once

#include "general.h"

class Line {
public:
    bool valid;
    bool dirty;
    bool is_high_priority;
    vector<bool> tag;
    vector<bool> data;
    vector<bool> access_order;
    vector<bool> ttl;

    Line();
    Line(int tag_size_bits, int data_size_bits, int ttl_size_bits, int pos_size_bits);
    void print_line();

};