#pragma once

#include<bits/stdc++.h>
using namespace std;

int adder(vector<bool>&a, vector<bool>&b, vector<bool>&ans, int &clock);
/*
    a,b : 32-bit floating point representations
    ans : 32-bit floating point represenation of (a+b)
        (a+b) is calculated, and passed reference ans is set equal to it
    clock : no of clock cycles
    Returns: success_code, where
        success_code = 0 : successful
        success_code = 1 : underflow
        success_code = 2 : overflow
*/