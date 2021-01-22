#include "Set.h"

Set::Set() {
    this->T = 0;
    this->a = 0;
    this->set_index = vector<bool>();
    this->lines = vector<Line>();
}

Set::Set(int a, int T, int tag_size_bits, int data_size_bits, vector<bool> set_index) {
    this->T = T;
    this->a = a;
    this->lines = vector<Line>(a);
    this->set_index = set_index;
    for (int i = 0; i < a; ++i)
        this->lines[i] = Line(tag_size_bits, data_size_bits, n_bits(T), n_bits(a));
}

bool Set::write(vector<bool> &tag, vector<bool> &data, bool &is_hit) {
    // find position of tag match
    int pos = -1;
    for (int i = 0; i < a; ++i)
        if (lines[i].valid && lines[i].tag == tag)
            pos = i;

    // HIT
    is_hit = (pos>=0);
    if (is_hit) {
        lines[pos].data = data;

        lines[pos].is_high_priority = true;
        lines[pos].ttl = int_to_binary(T, n_bits(T));

        lines[pos].dirty = true;

        // re adjusting values of group position for LRU
        for (int i = 0; i < a; ++i) {
            if (lines[i].valid && compare(lines[i].access_order, lines[pos].access_order) == -1)
                add(lines[i].access_order, 1);
        }
        lines[pos].access_order = int_to_binary(1, n_bits(a));

        return true;
    }

    // MISS
    int p = get_appropriate_pos();
    bool valid_before = lines[p].valid;

    if (lines[p].valid && lines[p].dirty) {
        vector<bool> writeAddress = getAddress(lines[p].tag, set_index);
        if (!mainMemory->write(writeAddress, lines[p].data)) {
            cout << "Error writing dirty block back to main memory\n";
            return false;
        }
    }

    lines[p].valid = true;
    lines[p].dirty = true;
    lines[p].is_high_priority = false;
    lines[p].tag = tag;
    lines[p].data = data;
    lines[p].ttl = int_to_binary(0, n_bits(T));

    // re adjusting values of group position for LRU
    for (int i = 0; i < a; ++i)
        if (lines[i].valid && (!valid_before ||
                               compare(lines[i].access_order, lines[p].access_order) == -1))
            add(lines[i].access_order, 1);

    lines[p].access_order = int_to_binary(1, n_bits(a));

    return true;
}

vector<bool> Set::read(vector<bool> &tag, vector<bool> &address, bool &is_hit) {
    // find position of tag match
    int pos = -1;
    for (int i = 0; i < a; ++i)
        if (lines[i].valid && lines[i].tag == tag)
            pos = i;

    // HIT
    is_hit = pos >= 0;
    if (is_hit) {

        lines[pos].is_high_priority = true;
        lines[pos].ttl = int_to_binary(T, n_bits(T));

        // re adjusting values of group position for LRU
        for (int i = 0; i < a; ++i) {
            if (lines[i].valid && compare(lines[i].access_order, lines[pos].access_order) == -1)
                add(lines[i].access_order, 1);
        }
        lines[pos].access_order = int_to_binary(1, n_bits(a));

        return lines[pos].data;
    }

    // MISS
    int p = get_appropriate_pos();
    bool valid_before = lines[p].valid;

    if (lines[p].valid && lines[p].dirty) {
        vector<bool> writeAddress = getAddress(lines[p].tag, set_index);
        if (!mainMemory->write(writeAddress, lines[p].data)) {
            cout << "Error writing dirty block back to main memory\n";
            return vector<bool>();
        }
    }
    vector<bool> memoryData = mainMemory->read(address);
    if (memoryData.empty()) {
        cout << "Error reading from main memory\n";
        return vector<bool>();
    }
    lines[p].data = memoryData;
    lines[p].valid = true;
    lines[p].dirty = false;
    lines[p].is_high_priority = false;
    lines[p].tag = tag;
    lines[p].ttl = int_to_binary(0, n_bits(T));

    // re adjusting values of group position for LRU
    for (int i = 0; i < a; ++i)
        if (lines[i].valid && (!valid_before ||
                               compare(lines[i].access_order, lines[p].access_order) == -1))
            add(lines[i].access_order, 1);

    lines[p].access_order = int_to_binary(1, n_bits(a));

    return lines[p].data;
}

int Set::get_appropriate_pos() {
    int pos = -1;
    bool foundEmpty = false, foundLow = false;
    vector<bool> highest_group_pos(n_bits(a), false);
    for (int i = 0; i < a; ++i)
        if (!foundEmpty) {
            if (!lines[i].valid) {
                foundEmpty = true;
                pos = i;
            } else if (!(lines[i].is_high_priority && foundLow)) {
                if (!lines[i].is_high_priority && !foundLow) {
                    foundLow = true;
                    pos = i;
                    highest_group_pos = lines[i].access_order;
                } else if (compare(lines[i].access_order, highest_group_pos) == 1) {
                    pos = i;
                    highest_group_pos = lines[i].access_order;
                }
            }
        }
    return pos;
}

void Set::update_groups() {
    vector<bool> zero = int_to_binary(0, n_bits(T));
    for (int i = 0; i < a; ++i)
        if (lines[i].valid && lines[i].is_high_priority) {
            if (compare(lines[i].ttl, zero) == 0)
                lines[i].is_high_priority = false;
            else add(lines[i].ttl, -1);
        }
}

void Set::print_set() {
    for (Line &l : lines)
        l.print_line();
}