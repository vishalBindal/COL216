#pragma once

#include "Set.h"

class Cache {
private:
    vector<Set> cache;

    // to read address and identify set
    int set_bits_start;
    int set_bits_end;
    void update_groups();

public:
    Cache(int size_cache, int size_block, int a, int T, int mm_address_bits);
    bool write(vector<bool> address, vector<bool> data, bool &is_hit);
    vector<bool> read(vector<bool> address, bool &is_hit);
    void print_cache();
};
