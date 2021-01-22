#include "general.h"

int n_bits(unsigned int n) {
    int b = 0;
    while (n > 0) {
        n >>= 1u;
        ++b;
    }
    return b;
}

void twos_complement(vector<bool> &v)
{
    for(auto && i : v)
        i = !i;
    bool carry = true;
    for (int i = v.size()-1; i >= 0 && carry; --i) {
        carry = v[i];
        v[i] = !v[i];
    }
}

long long binary_to_int(vector<bool> v, bool unsigned_number)
{
    bool neg = false;
    if(!unsigned_number && v[0])
    {
        neg = true;
        twos_complement(v);
    }
    long long a = 0, m = 1;
    for(size_t i=0;i<v.size();++i)
    {
        a += (int) v[v.size()-1-i] * m;
        m*=2;
    }
    if(neg)
        a *= -1;
    return a;
}

vector<bool> int_to_binary(long long a, int size, bool unsigned_number)
{
    vector<bool> v(size, false);

    bool neg = false;
    if(a<0)
    {
        neg = true;
        a *= -1;
    }
    for(int i=0; i<size && a>0;++i)
    {
        v[size-1-i] = a&1;
        a>>=1;
    }
    if(a>0 || (!unsigned_number && !neg && v[0]))
        return vector<bool>();
    if(neg)
        twos_complement(v);
    return v;
}

void add(vector<bool> &v, int i)
{
    vector<bool> a = int_to_binary(binary_to_int(v) + i, v.size());
    v = a;
}

int compare(vector<bool> &v1, vector<bool> &v2)
{
    for(size_t i=0;i<v1.size();++i)
        if(v1[i] && !v2[i])
            return 1;
        else if(v2[i] && !v1[i])
            return -1;
    return 0;
}

vector<bool> getAddress(vector<bool> &tag, vector<bool> &set_index)
{
    vector<bool> address(tag.size() + set_index.size());

    int k = 0;
    for(bool b: tag)
        address[k++] = b;
    for(bool b: set_index)
        address[k++] = b;
    return address;
}

string dash(int n)
{
    string a;
    for(int i=0;i<n;++i)
        a.push_back('-');
    return a;
}
