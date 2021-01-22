#include "Cache.h"
#include <algorithm>

// To run
// set below parameters
int cache_size = 64, block_size = 8, a = 1, T = 5;
// put appropriate test in main
// To generate input file, set to true. To test, set to false
bool generate_input_file = false;
// make test
// ./test

int read_hits = 0, write_hits = 0, reads = 0, writes = 0;
Cache *cache;
int mm_address_bits = 16 - (n_bits(block_size-1)+3);

bool show_details = false;

int read(int addr)
{
    if(generate_input_file)
        cout<<addr<<",R\n";
    bool is_hit;
    vector<bool> address = int_to_binary(addr, mm_address_bits);
    vector<bool> data = cache->read(address, is_hit);
    ++reads;
    if(is_hit) ++read_hits;
    return binary_to_int(data);
}

void write(int addr, int data)
{   
    if(generate_input_file)
        cout<<addr<<",W,"<<data<<"\n";
    bool is_hit;
    vector<bool> address = int_to_binary(addr, mm_address_bits);
    vector<bool> d = int_to_binary(data, 8*block_size);
    cache->write(address, d, is_hit);
    ++writes;
    if(is_hit) ++write_hits;
}

void test1_fibonacci(int n);
void test2_sumFirstN(int n);
void test3_qsort(int n);

int main()
{
    int mem_size = (1<<16)/(block_size*8);
    mainMemory = new Main_memory(mem_size, block_size);
    cache = new Cache(cache_size, block_size, a, T, mm_address_bits);
    
    if(generate_input_file)
        cout<<cache_size<<endl<<block_size<<endl<<a<<endl<<T<<endl;
    
    test3_qsort(100);

    if(!generate_input_file)
        cout<<"Hit ratio: "<<(float)(read_hits + write_hits)/(float)(reads + writes)<<endl;

    return 0;
}

void test1_fibonacci(int n)
{
    int a[n];
    
    a[0] = 1;
    write(0,1);
    a[1] = 1;
    write(1,1);

    for(int i=2;i<n;++i)
    {
        a[i] = a[i-1] + a[i-2];
        write(i, read(i-1) + read(i-2));
    }

    if(!generate_input_file)
    {
        cache->print_cache();
        cout<<n<<"th fibonacci number: "<<a[n-1]<<"\n";
        cout<<"According to cache: "<<read(n-1)<<"\n";
        if(read(n-1) == a[n-1])
            cout<<"PASS!\n";
        else cout<<"FAIL!\n";
    }
}


void test2_sumFirstN(int n)
{
    int a[n];

    for(int i=0;i<n;++i)
    {
        a[i] = i+1;
        write(i, i+1);
    }
    for(int i=1;i<n;++i)
    {
        a[i] += a[i-1];
        write(i, read(i) + read(i-1));
    }

    if(!generate_input_file)
    {
        cache->print_cache();
        cout<<"Sum of first "<<n<<" natural numbers: "<<a[n-1]<<"\n";
        cout<<"According to cache: "<<read(n-1)<<"\n";
        if(read(n-1) == a[n-1])
            cout<<"PASS!\n";
        else cout<<"FAIL!\n";
    }
}

void swap(int i, int j) 
{ 
    int temp = read(i);
    write(i, read(j));
    write(j, temp); 
} 

int partition (int low, int high) 
{ 
	int pivot = read(high);
	int i = low-1; 
	for (int j=low; j<=high-1; j++) 
	{ 

		if (read(j) < pivot) 
		{ 
			i++; 
			swap(i, j); 
		} 
	} 
	swap(i + 1, high); 
	return (i + 1); 
} 

void qsort_cache(int low, int high) 
{ 
	if (low < high) 
	{ 
		int pi = partition(low, high); 
		qsort_cache(low, pi - 1); 
		qsort_cache(pi + 1, high); 
	} 
} 


void test3_qsort(int n)
{
    vector<int> a(n);
    for(int i=0;i<n;++i)
        a[i] = i+1;

    random_shuffle(a.begin(), a.end());

    for(int i=0;i<n;++i)
        write(i, a[i]);

    qsort_cache(0, n-1);

    if(!generate_input_file)
    {
        cache->print_cache();
        cout<<"Initial array:\n";
        for(int i=0;i<n;++i)
            cout<<a[i]<<" ";
        cout<<endl;

        cout<<"Sorted array:\n";
        for(int i=0;i<n;++i)
            cout<<read(i)<<" ";
        cout<<endl;

        bool success = true;
        for(int i=0;i<n;++i)
            if(read(i)!=(i+1))
                success = false;
        if(success)
            cout<<"PASS!\n";
        else cout<<"FAIL!\n";
    }
}
