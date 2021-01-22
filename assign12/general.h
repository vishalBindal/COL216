#pragma once

#include <iostream>
#include <vector>
#include <iomanip>
using namespace std;

int n_bits(unsigned int n);
long long binary_to_int(vector<bool> v, bool unsigned_number = true);
vector<bool> int_to_binary(long long a, int size, bool unsigned_number = true);
void add(vector<bool> &v, int i);
int compare(vector<bool> &v1, vector<bool> &v2);
vector<bool> getAddress(vector<bool> &tag, vector<bool> &set_index);
string dash(int n);

extern bool show_details;