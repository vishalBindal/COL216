#include "Adder.h"

bool show_clock = true;

void parse_line(string line, vector<vector<bool>>&a_v, vector<vector<bool>>&b_v)
{
    int n = line.length();
    
    int i = 0;
    // remove whitespaces at beginning
    while(i<n && line[i]==' ')
        ++i;
    // return if empty after trimming whitespaces
    if(i==n)
        return;
    // return if comment
    if(line[i]!='1' && line[i]!='0')
        return;

    vector<bool> x(32), y(32);
    
    for(int j=0;j<32;++j,++i)
        x[j] = (line[i] == '1');

    // traverse whitespace(s) between numbers
    while(i<n && line[i]==' ')
        ++i;
    // return if no second number
    if(i==n)
        return;
    
    for(int j=0;j<32;++j,++i)
        y[j] = (line[i]=='1');

    a_v.push_back(x);
    b_v.push_back(y);
}

void read_input(const string &input_path, vector<vector<bool>>&a_v, vector<vector<bool>>&b_v)
{
    fstream file;
    string line;
    file.open(input_path, ios::in);
    while(getline(file, line))
        parse_line(line, a_v, b_v);
}

int main(int argc, char** argv)
{
    string input_path = "input.txt";
    if(argc==2)
        input_path = argv[1];

    vector<vector<bool>> a_v;
    vector<vector<bool>> b_v;
    read_input(input_path, a_v, b_v);
    
    int clock = 0;
    int total_cycles = 0;

    for(int i=0; i<a_v.size(); ++i)
    {
        if(show_clock)
        {
            cout<<"Test case no."<<i+1<<endl;
        }
        vector<bool> ans;
        clock = 0;
        // adder defined in Adder.h, Adder.cpp
        bool success_code = adder(a_v[i], b_v[i], ans, clock);
        if(success_code) {
            for (bool ai: ans)
                cout << (int) ai;
            cout << endl;
        }
        else
            cout<<"Overflow\n";
        total_cycles += clock;
        if(show_clock)
        {
            cout<<"Clock cycles: "<<clock<<"\n\n";
        }
    }
    if(show_clock)
        cout<<"\nTotal clock cycles: "<<total_cycles<<endl;
    return 0;
}
