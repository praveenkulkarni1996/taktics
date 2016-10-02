#include <iostream>
#include <vector>
#include <string>
#include <cassert>
using namespace std;

const int N = 5;

typedef uint8_t int8;
typedef uint16_t int16;
typedef uint32_t int32;
typedef uint64_t int64;

typedef int8 Point;

typedef string Move;
typedef vector<Move> Moves;

enum Stones {
    BLACK_FLAT,
    BLACK_WALL,
    BLACK_CAP,
    BLACK_CRUSH,
    WHITE_FLAT,
    WHITE_WALL,
    WHITE_CAP,
    WHITE_CRUSH
};

class Board {

public:
    vector<Stones> board[5][5];
    int flats_rem = 21;
    int caps_rem = 1;
    
    bool empty(int x, int y) {
        assert(0 <= x and x < N);
        assert(0 <= y and y < N);
        return board[x][y].empty();
    }

    bool white_wall(int x, int y) {
        return (not empty(x, y) and board[x][y].back() == WHITE_WALL);
    }
    bool white_flat(int x, int y) {
        return (not empty(x, y) and board[x][y].back() == WHITE_FLAT);
    }
    bool white_cap(int x, int y) {
        return (not empty(x, y) and board[x][y].back() == WHITE_CAP);
    }
    bool black_wall(int x, int y) {
        return (not empty(x, y) and board[x][y].back() == BLACK_WALL);
    }
    bool black_flat(int x, int y) {
        return (not empty(x, y) and board[x][y].back() == BLACK_FLAT);
    }
    bool black_cap(int x, int y) {
        return (not empty(x, y) and board[x][y].back() == BLACK_CAP);
    }

    int height(int x, int y) {
        return board[x][y].size();
    }
};

void generate_moves(Board &board);
void generate_placement_moves(Board &board, Moves &moves);
void generate_motion_moves(Board &board, Moves &moves);
string make_sqr(int x, int y);
void motion(const char dir, int x, int y, string prefix, Board &board, Moves &moves, int ht);
void cap_motion(const char dir, int x, int y, string prefix, Board &board, Moves &moves, int ht);

inline string make_sqr(int x, int y) {
    assert(0 <= x && x < N);
    assert(0 <= y && y < N);
    string s;
    s += ('a' + x);
    s += ('1' + y);
    return s;
}

inline bool out_of_bounds(int x, int y) {
    return not(0 <= x and x < N and 0 <= y and y < N);
}

void generate_moves(Board &board) {
    vector<Move> moves;
    // generate_placement_moves(board, moves);
    generate_motion_moves(board, moves);
    cout << "[";
    for(string move : moves) {
        cout << move << ", ";
    }
    cout << "]";
}


void generate_placement_moves(Board &board, Moves &moves) {
    for(int x = 0; x < N; ++x) {
        for(int y = 0; y < N; ++y) {
            if(board.empty(x, y)) {
                string s = make_sqr(x, y);
                if(board.flats_rem > 0) {
                    moves.push_back("W" + s);
                    moves.push_back("F" + s);
                }
                if(board.caps_rem > 0) {
                    moves.push_back("C" + s);
                }
            }
        }
    }
}

inline int next_x(int x, char dir) {
    switch(dir) {
        case '<': return (x-1);
        case '>': return (x+1);
        case '+':
        case '-': return x;
        default: assert(false);
    }
}

inline int next_y(int y, char dir) {
    switch(dir) {
        case '-': return (y-1);
        case '+': return (y+1);
        case '<':
        case '>': return y;
        default: assert(false);
    }
}
void generate_motion_moves(Board &board, Moves &moves) {
    const string dirs = "+-<>";

    for(int dir=0; dir < 4; ++dir) {
        for(int x = 0; x < N; ++x) {
            for(int y = 0; y < N; ++y) {
                if(board.empty(x, y)) continue;
                int xx = next_x(x, dirs[dir]);
                int yy = next_y(y, dirs[dir]);
                if(board.white_flat(x, y) || board.white_wall(x, y)) {
                    int H = board.height(x, y);
                    for(int h = 1; h <= min(H, N); ++h) {
                        string prefix = to_string(h) + make_sqr(x, y) + dirs[dir];
                        motion(dirs[dir], xx, yy, prefix, board, moves, h);
                    }
                    cout << "works\n";
                }
                else if(board.white_cap(x, y)) {
                    int H = board.height(x, y);
                    for(int h = 1; h <= min(H, N); ++h) {
                        string prefix = to_string(h) + make_sqr(x, y) + dirs[dir];
                        cap_motion(dirs[dir], xx, yy, prefix, board, moves, h);
                    }
                }
            }
        }
    }
}

void motion(const char dir, int x, int y, string prefix, Board &board, Moves &moves, int ht) {
    if(out_of_bounds(x, y)) return;
    if(board.white_cap(x, y) || board.black_cap(x, y)) return;
    if(board.white_wall(x, y) || board.black_wall(x, y)) return;
    moves.push_back(prefix + to_string(ht));
    int xx = next_x(x, dir);
    int yy = next_y(y, dir);
    for(int h = 1; h < ht; ++h) {
        motion(dir, xx, yy, prefix + to_string(h), board, moves, ht - h);
    }
}

void cap_motion(const char dir, int x, int y, string prefix, Board &board, Moves &moves, int ht) {
    if(out_of_bounds(x, y)) return;
    if(board.white_cap(x, y) || board.black_cap(x, y)) return;
    if((board.white_wall(x, y) || board.black_wall(x, y)) and ht > 1) return;
    moves.push_back(prefix + to_string(ht));
    int xx = next_x(x, dir);
    int yy = next_y(y, dir);
    for(int h = 1; h < ht; ++h) {
        cap_motion(dir, xx, yy, prefix + to_string(h), board, moves, ht - h);
    }
}

int main() {
    Board board;
    board.board[0][0].push_back(WHITE_CAP);
    board.board[1][0].push_back(BLACK_WALL);
    generate_moves(board);
}