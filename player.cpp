#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include "utility.h"
using namespace std;

const int N = 5;

typedef uint8_t int8;
typedef uint16_t int16;
typedef uint32_t int32;
typedef uint64_t int64;

typedef int8 Point;

typedef string Move;
typedef vector<Move> Moves;

inline pair<int, int> make_xy(char x, char y) {
    return make_pair(x-'a', y-1);
}

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
    int white_flats_rem = 21;
    int white_caps_rem = 1;
    int black_flats_rem = 21;
    int black_caps_rem = 1;

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
    bool white(int x, int y) {
        return white_flat(x, y) || white_wall(x, y) || white_cap(x, y);
    }
    bool black(int x, int y) {
        return black_flat(x, y) || black_wall(x, y) || black_cap(x, y);
    }
    int height(int x, int y) {
        return board[x][y].size();
    }

    void perform_placement(Move move, bool white) {
        const pair<int, int> xy = make_xy(move[1], move[2]);
        const int x = xy.first; 
        const int y = xy.second;
        if(white) {
            switch(move[0]) {
                case 'F': board[x][y].push_back(WHITE_FLAT); black_flats_rem--; break;
                case 'W': board[x][y].push_back(WHITE_WALL); black_flats_rem--; break;
                case 'C': board[x][y].push_back(WHITE_CAP);  black_caps_rem--; break;
                default : assert(false);
            }
        } else {
            switch(move[0]) {
                case 'F': board[x][y].push_back(BLACK_FLAT); black_flats_rem--; break;
                case 'W': board[x][y].push_back(BLACK_WALL); black_flats_rem--; break;
                case 'C': board[x][y].push_back(BLACK_CAP);  black_caps_rem--; break;
                default : assert(false);
            }
        }
    }
};

void generate_moves(Board &board, bool white);
void generate_placement_moves(Board &board, Moves &moves, bool white);
void generate_motion_moves(Board &board, Moves &moves, bool white);
void motion(const char dir, int x, int y, string prefix, Board &board, Moves &moves, int ht);
void cap_motion(const char dir, int x, int y, string prefix, Board &board, Moves &moves, int ht);


void generate_moves(Board &board, bool white) {
    /* generates a list of moves for either player 
       and prints them out 
    */
    vector<Move> moves;
    // generate_placement_moves(board, moves, white);
    generate_motion_moves(board, moves, white);
    cout << "[";
    for(string move : moves) {
        cout << move << ", ";
    }
    cout << "]";
}


void generate_placement_moves(Board &board, Moves &moves, bool white) {
    /* generates moves to place pieces for white and black */
    const int flats = (white) ? board.white_flats_rem : board.black_flats_rem;
    const int caps = (white) ? board.white_caps_rem : board.black_caps_rem;
    for(int x = 0; x < N; ++x) {
        for(int y = 0; y < N; ++y) {
            if(board.empty(x, y)) {
                string s = make_sqr(x, y);
                if(flats > 0) {
                    moves.push_back("W" + s);
                    moves.push_back("F" + s);
                }
                if(caps > 0) {
                    moves.push_back("C" + s);
                }
            }
        }
    }
}

void generate_motion_moves(Board &board, Moves &moves, bool white) {
    /* generate the moves for moving stacks for either player */
    const string dirs = "+-<>";
    for(int dir = 0; dir < 4; ++dir) {
        for(int x = 0; x < N; ++x) {
            for(int y = 0; y < N; ++y) {
                if(board.empty(x, y)) continue;
                const int xx = next_x(x, dirs[dir]);
                const int yy = next_y(y, dirs[dir]);
                /* some predicates for testing the exitence of players' stack */
                const bool white_cap = (white and board.white_cap(x, y));
                const bool black_cap = ((not white) and board.black_cap(x, y));
                const bool white_stack = (white and (board.white_flat(x, y) or board.white_wall(x, y)));
                const bool black_stack = ((not white) and ((board.black_flat(x, y) or board.black_wall(x, y))));
                const int H = board.height(x, y);
                /* because there is a carry limit */
                for(int h = 1; h <= min(H, N); ++h) {
                    string prefix = to_string(h) + make_sqr(x, y) + dirs[dir];
                    if(white_stack or black_stack) {
                        /* there is a stack for the player which he might move */
                        motion(dirs[dir], xx, yy, prefix, board, moves, h);
                    }
                    else if(white_cap or black_cap) {
                        /* there is a capstone stack for the player which he might move */
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

void perform_move(Board &board, const Move move, bool white=true) {
    if(move[0] == 'F' || move[0] == 'S' || move[0] == 'C') 
        board.perform_placement(move, white);
    else ; //perform_motion(move, white);
}

int main() {
    Board board;
    board.board[0][0].push_back(WHITE_WALL);
    board.board[1][0].push_back(BLACK_CAP);
    generate_moves(board, false);

}