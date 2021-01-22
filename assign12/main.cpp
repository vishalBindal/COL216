#include "Cache.h"
#include <fstream>
#include <string>

bool show_details = false;
bool wait = false;

int get_first_num(string &s)
{
    int n = 0;
    int k = 0;
    while(k<s.length() && s[k]==' ')
        ++k;
    if(k==s.length() || !(s[k]-'0'>=0 && s[k]-'0'<10))
        return -1;
    while(k<s.length() && s[k]-'0'>=0 && s[k]-'0'<10)
    {
        n = n*10 + (s[k]-'0');
        ++k;
    }
    return n;
}

bool get_param(ifstream &input_file, int &param)
{
    string line;
    do {
        if(!getline(input_file, line))
        {
            cout <<"Cache parameter not found in input file\n";
            return false;
        }
        param = get_first_num(line);
    } while(param < 0);
    return true;
}

bool is_delim(char c, const string &delimiter) {
    for (char i : delimiter)
        if (c == i)
            return true;
    return false;
}

vector<string> tokenize(string &s, string delimiter)
{
    delimiter += "\n\r";
    int k = 0;
    vector<string> tokens;
    while(k<s.length())
    {
        string token;
        while(k<s.length() && is_delim(s[k], delimiter))
            ++k;
        if(k==s.length())
            break;
        if(s[k]=='#')
            return tokens;
        while(k<s.length() && !is_delim(s[k], delimiter))
        {
            token.push_back(s[k]);
            ++k;
        }
        tokens.push_back(token);
    }
    return tokens;
}

bool get_access_request(ifstream &input_file, bool &read, vector<bool> &addr,
        vector<bool> &data, int addr_bits, int data_bits, bool &err)
{
    string line;
    do {
        if(!getline(input_file, line))
            return false;
        vector<string> tokens = tokenize(line, ", ");
        if(tokens.size() < 2)
            continue;
        long long addr_ll = stoll(tokens[0]);
        if(addr_ll < 0 || addr_ll >= (1<<addr_bits))
        {
            cout << "\nInvalid address\n";
            err = true;
            return false;
        }
        addr = int_to_binary(stoll(tokens[0]), addr_bits);
        read = tokens[1]=="R";
        if(!read) {
            if (tokens[1] != "W" || tokens.size() < 3)
                continue;
            data = int_to_binary(stoll(tokens[2]), data_bits, false);
            if(data.empty())
            {
                cout << "\nData overflow for given block size\n";
                err = true;
                return false;
            }
        }
        return true;
    } while(true);
}

int main(int argc, char** argv) {
    ifstream input_file;
    string input_path = "input.txt";
    if(argc==2)
        input_path = argv[1];
    input_file.open(input_path, ios::in);
    if(!input_file.is_open())
    {
        cout << "Error in opening input file\n";
        return 1;
    }

    int cache_size, block_size, a, T;
    if(!get_param(input_file, cache_size))
        return 1;
    if(!get_param(input_file, block_size))
        return 1;
    if(!get_param(input_file, a))
        return 1;
    if(!get_param(input_file, T))
        return 1;

    // let main memory contain 2^16 bits of data
    int mem_size = (1<<16)/(block_size*8);
    int mm_address_bits = 16 - (n_bits(block_size-1)+3);

    // initialise main memory and cache
    mainMemory = new Main_memory(mem_size, block_size);
    Cache cache(cache_size, block_size, a, T, mm_address_bits);

    // Cache operations
    bool read;
    vector<bool> addr, data;

    int no_of_reads = 0, no_of_writes = 0;
    int read_hits = 0, write_hits = 0;
    bool is_hit, err=false;

    while(get_access_request(input_file, read, addr, data,
            mm_address_bits, 8*block_size,err))
    {
        if(read)
        {
            ++no_of_reads;
            if(show_details)
                cout<<"\nRead from address "<<binary_to_int(addr)<<"\n";
            vector<bool> cache_data = cache.read(addr, is_hit);
            if(cache_data.empty())
            {
                cout << "Cache read unsuccessful\n";
                return 1;
            }
            if(is_hit)
                ++read_hits;
        }
        else
        {
            ++no_of_writes;
            if(show_details)
                cout<<"\nWrite data "<<binary_to_int(data, false)
                    <<" to address "<<binary_to_int(addr)<<"\n";
            if(!cache.write(addr, data, is_hit))
            {
                cout << "Cache write unsuccessful\n";
                return 1;
            }
            if(is_hit)
                ++write_hits;
        }
        if(show_details) {
            cache.print_cache();
            if(wait)
                getchar();
        }
    }
    input_file.close();

    if(err)
        return 1;
    cout<<"\n"<<dash(37)<<"RESULTS"<<dash(41)<<"\n";
    cache.print_cache();
    cout<<"Cache statistics:\n";
    cout<<"Number of Accesses = "<<no_of_reads + no_of_writes<<"\n";
    cout<<"Number of Reads = "<<no_of_reads<<"\n";
    cout<<"Number of Read Hits = "<<read_hits<<"\n";
    cout<<"Number of Read Misses = "<<no_of_reads - read_hits<<"\n";
    cout<<"Number of Writes = "<<no_of_writes<<"\n";
    cout<<"Number of Write Hits = "<<write_hits<<"\n";
    cout<<"Number of Write Misses = "<<no_of_writes - write_hits<<"\n";
    cout<<"Hit Ratio = "<<(float)(read_hits + write_hits)/(float)(no_of_reads + no_of_writes)<<"\n";
    return 0;
}