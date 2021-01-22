#include "Line.h"

Line::Line()
{
    this->valid = false;
    this->dirty = false;
    this->is_high_priority = false;
    this->data = vector<bool>();
    this->tag = vector<bool>();
    this->access_order = vector<bool>();
    this->ttl = vector<bool>();
}
Line::Line(int tag_size_bits, int data_size_bits, int ttl_size_bits, int pos_size_bits)
{
    this->valid = false;
    this->dirty = false;
    this->is_high_priority = false;
    this->data = vector<bool>(data_size_bits, false);
    this->tag = vector<bool>(tag_size_bits, false);
    this->access_order = vector<bool>(pos_size_bits, false);
    this->ttl = vector<bool>(ttl_size_bits, false);
}

void Line::print_line()
{
    string sep = " ";
    if(valid)
        cout<<"\033[32m";
    else
        cout<<"\033[31m";
    cout<<setw(5)<<(int)valid<<sep<<setw(3)<<(int)is_high_priority<<sep;
    cout<<setw(5)<<(int)dirty<<sep;
    cout<<setw(16)<<binary_to_int(tag)<<sep<<setw(20)<<binary_to_int(data, false)<<sep;
    cout<<setw(13)<<binary_to_int(access_order)<<sep<<setw(5)<<binary_to_int(ttl)<<sep;
    cout<<"\033[0m\n";
}