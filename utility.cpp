#include "utility.h"
#include <cassert>
#include <iostream>
using namespace std;

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

pair<int, int> make_xy(char x, char y) {
    return make_pair(x-'a', y-'1');
}

void print_moves(Moves moves) {
    cout << "[";
    for(const Move &move : moves) {
        cout << move << ", ";
    }
    cout << "]";
}

bool check_white(const Stones &stone) {
    return (stone == WHITE_FLAT || stone == WHITE_WALL 
        || stone == WHITE_CAP || stone == WHITE_CRUSH);
}

bool check_black(const Stones &stone) {
    return (stone == BLACK_FLAT || stone == BLACK_WALL 
        || stone == BLACK_CAP || stone == BLACK_CRUSH);
}

