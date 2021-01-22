#include "Adder.h"

bool zero, NaN, inf_p, inf_n, denormal;

void parse_float_rep(vector<bool> &x, bool &s, int &exp, vector<bool> &f) {
    s = x[0];

    exp = 0;
    for (int i = 8; i > 0; --i)
        exp += (int) x[i] * (1 << (8 - i));

    f = vector<bool>(26, false);
    // implicit bit: 0 for denormalized number, 1 otherwise
    f[1] = exp > 0;

    // f[2..24] = x[9..31]
    for (int i = 9; i < 32; ++i)
        f[i - 7] = x[i];

    if (exp == 255 && !NaN) {
        for (int i = 9; i < 32; ++i)
            if (x[i]) {
                NaN = true;
                break;
            }
        if (!NaN)
        {
            if(s) inf_n = true;
            else inf_p = true;
            if(inf_n && inf_p) NaN = true;
        }
    }
}

void shift_right(vector<bool> &f, int no_of_shifts)
/*
 * Shifting f right by no_of_shifts "in-place"
 */
{
    for (int i = f.size() - 1; i >= 0; --i) {
        if (i - no_of_shifts >= 0)
            f[i] = f[i - no_of_shifts];
        else
            f[i] = false;
    }
}

void shift_left(vector<bool> &f, int no_of_shifts)
/*
 * Shifting f right by no_of_shifts "in-place"
 */
{
    for (int i = 0; i < f.size(); ++i) {
        if (i + no_of_shifts < f.size())
            f[i] = f[i + no_of_shifts];
        else
            f[i] = false;
    }
}

vector<bool> add(vector<bool> &f_a, vector<bool> &f_b) {
    vector<bool> f_ans(26);
    bool carry = false;
    for (int i = 25; i >= 0; --i) {
        f_ans[i] = f_a[i] ^ f_b[i] ^ carry;
        carry = (f_a[i] && f_b[i]) || (f_a[i] && carry) || (f_b[i] && carry);
    }
    return f_ans;
}

int compare(vector<bool> &f_a, vector<bool> &f_b) {
    for (int i = 0; i < 26; ++i)
        if (f_a[i] && !f_b[i])
            return 1;
        else if (f_b[i] && !f_a[i])
            return -1;
    return 0;
}

vector<bool> sub(vector<bool> f_a, vector<bool> f_b, bool &s_ans, bool s_a, bool s_b) {
    vector<bool> gt, sm;
    int c = compare(f_a, f_b);
    if (c == 1) {
        gt = f_a;
        sm = f_b;
        s_ans = s_a;
    } else if (c == -1) {
        gt = f_b;
        sm = f_a;
        s_ans = s_b;
    } else {
        s_ans = false;
        zero = true;
        return vector<bool>(26, false);
    }

    bool carry = false;
    vector<bool> f_ans(26);
    for (int i = 25; i >= 0; --i) {
        f_ans[i] = gt[i] ^ sm[i] ^ carry;
        carry = ((carry || sm[i]) && !gt[i]) || (carry && sm[i] && gt[i]);
    }
    return f_ans;
}

void normalize(vector<bool> &f, int &exp) {
    if(zero)
        exp = 0;
    else if (f[0]) {
        shift_right(f, 1);
        ++exp;
    } else if (!f[1]) {
        int shifts_needed = 1, j=2;
        while(j<26 && !f[j])
        {
            ++shifts_needed;
            ++j;
        }

        // handle underflow using denormalized representation
        if(shifts_needed >= exp)
        {
            if(exp>2)
                shift_left(f, exp-1);
            exp = 0;
            denormal = true;
        }
        else
        {
            shift_left(f, shifts_needed);
            exp -= shifts_needed;
        }
    } else if(exp == 0) // sum of 2 denormal nos becoming normal
    {
        exp = 1;
    }
}

bool check_overflow(int &exp)
{
    if (NaN || inf_n || inf_p)
        return false;
    return (exp > 254);
}

void round(vector<bool> &f, int &exp) {
    if (f[25]) // need to round up
    {
        bool carry = true;
        for (int i = 24; i >= 0 && carry; --i) {
            carry = f[i];
            f[i] = !f[i];
        }
    }
    if(f[1])
        denormal = false;
}

bool is_normalized(vector<bool> &f, int &exp) {
    return (NaN || inf_n || inf_p || zero || denormal || (!f[0] && f[1] && exp>0 && exp<255));
}

vector<bool> convert_to_float_rep(vector<bool> &f, int exp, bool s) {
    vector<bool> float_num(32);
    float_num[0] = s;
    for (int i = 8; i >=1; --i) {
        float_num[i] = exp & 1;
        exp >>= 1;
    }
    for(int i=2; i<25; ++i)
        float_num[i+7] = f[i];
    return float_num;
}

int adder(vector<bool> &a, vector<bool> &b, vector<bool> &ans, int &clock)
// 0 : success, 1 : underflow, 2 : overflow
{
    NaN = false;
    zero = false;
    inf_p = false;
    inf_n = false;
    denormal = false;

    bool s_a, s_b, s_ans;
    int exp_a, exp_b, exp_ans;
    vector<bool> f_a, f_b, f_ans;

    parse_float_rep(a, s_a, exp_a, f_a);
    parse_float_rep(b, s_b, exp_b, f_b);

    // step 1
    if (!NaN && !inf_n && !inf_p) {
        if (exp_a > exp_b) {
            shift_right(f_b, exp_a - exp_b);
            exp_b = exp_a;
        } else if (exp_b > exp_a) {
            shift_right(f_a, exp_b - exp_a);
            exp_a = exp_b;
        }
    }
    ++clock;

    // step 2
    if (!NaN && !inf_n && !inf_p) {
        exp_ans = exp_a;
        if (s_a == s_b) {
            s_ans = s_a;
            f_ans = add(f_a, f_b);
        } else
            f_ans = sub(f_a, f_b, s_ans, s_a, s_b);
    } else {
        exp_ans = 255;
        if(NaN)
        {
            f_ans = vector<bool>(26, true);
            s_ans = false;
        }
        else
        {
            f_ans = vector<bool>(26, false);
            s_ans = inf_n;
        }

    }
    ++clock;

    bool fin = false;
    bool status_code;

    do {
        // step 3
        if(!NaN && !inf_n && !inf_p)
            normalize(f_ans, exp_ans);
        ++clock;

        status_code = !(check_overflow(exp_ans));

        if (status_code) {
            // step 4
            if(!NaN && !inf_n && !inf_p && !zero)
                round(f_ans, exp_ans);
            ++clock;

            if (is_normalized(f_ans, exp_ans))
                fin = true;
        } else
            fin = true;

    } while (!fin);

    ans = convert_to_float_rep(f_ans, exp_ans, s_ans);
    return status_code;
};