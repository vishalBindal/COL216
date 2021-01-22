#include "Cache.h"

Cache::Cache(int size_cache, int size_block, int a, int T, int mm_address_bits)
{
    int no_of_sets = (size_cache / size_block) / a;

    this->cache = vector<Set>(no_of_sets);
    this->set_bits_end = mm_address_bits - 1;
    this->set_bits_start = this->set_bits_end - n_bits(no_of_sets-1) + 1;

    for(int i=0;i<no_of_sets;++i)
    {
        vector<bool> set_index = int_to_binary(i, n_bits(no_of_sets - 1));
        this->cache[i] = Set(a, T, n_bits(this->set_bits_start), size_block*8, set_index);
    }
}

bool Cache::write(vector<bool> address, vector<bool> data, bool &is_hit) {
    vector<bool> set_number(address.begin() + set_bits_start, address.begin() + set_bits_end + 1);
    vector<bool> tag(address.begin(), address.begin() + set_bits_start);
    bool success = this->cache[binary_to_int(set_number)].write(tag, data, is_hit);
    update_groups();
    return success;
}

vector<bool> Cache::read(vector<bool> address, bool &is_hit) {
    vector<bool> set_number(address.begin() + set_bits_start, address.begin() + set_bits_end + 1);
    vector<bool> tag(address.begin(), address.begin() + set_bits_start);
    vector<bool> data = this->cache[binary_to_int(set_number)].read(tag, address, is_hit);
    update_groups();
    return data;
}

void Cache::update_groups()
{
    for(Set &s: cache)
        s.update_groups();
}
void Cache::print_cache()
{
    string sep = " ";
    cout<<"\nValid"<<sep<<"HPG"<<sep<<"Dirty"<<sep;
    cout<<setw(16)<<"Tag"<<sep<<setw(20)<<"Data"<<sep;
    cout<<setw(13)<<"Access-order"<<sep<<setw(5)<<"TTL"<<sep<<"\n";

    for(size_t i=0;i<cache.size();++i)
    {
        Set s = cache[i];

        string str = "[ SET "+to_string(i)+" ]";
        cout<<dash(37)<<str<<dash(48-(int)str.length())<<"\n";

        s.print_set();
    }
}
