#include "utility.h"
#include <cassert>

bool out_of_bounds(const int x, const int y, const int N) {
    return not(0 <= x and x < N and 0 <= y and y < N);
}

int next_x(int x, char dir) {
    switch(dir) {
        case '<': return (x-1);
        case '>': return (x+1);
        case '+':
        case '-': return x;
        default: assert(false);
    }
}

int next_y(int y, char dir) {
    switch(dir) {
        case '-': return (y-1);
        case '+': return (y+1);
        case '<':
        case '>': return y;
        default: assert(false);
    }
}

string make_sqr(int x, int y, int N) {
    assert(0 <= x && x < N);
    assert(0 <= y && y < N);
    string s;
    s += ('a' + x);
    s += ('1' + y);
    return s;
}