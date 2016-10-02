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

    bool perform_placement(Move move, bool white) {
        const pair<int, int> xy = make_xy(move[1], move[2]);
        const int x = xy.first; 
        const int y = xy.second;
        cout << "x = " << x << " y = " << y << "\n";
        if(white) {
            switch(move[0]) {
                case 'F': board[x][y].push_back(WHITE_FLAT); black_flats_rem--; break;
                case 'S': board[x][y].push_back(WHITE_WALL); black_flats_rem--; break;
                case 'C': board[x][y].push_back(WHITE_CAP);  black_caps_rem--; break;
                default : assert(false);
            }
        } else {
            switch(move[0]) {
                case 'F': board[x][y].push_back(BLACK_FLAT); black_flats_rem--; break;
                case 'S': board[x][y].push_back(BLACK_WALL); black_flats_rem--; break;
                case 'C': board[x][y].push_back(BLACK_CAP);  black_caps_rem--; break;
                default : assert(false);
            }
        }
        return false; // because you cannot crush in a placement
    }

    bool perform_motion(Move move, bool white) {
        // Assumes a valid move
        /* returns if you crushed or not */
        int h = move[0] - '0';
        bool crush = false;
        const pair<int, int> xy = make_xy(move[1], move[2]);
        int x = xy.first;
        int y = xy.second;

        assert(not board[x][y].empty());
        assert(this->white(x, y) == white);

        const char dir = move[3];
        cout << "x = " << x << " y = " << y << "\n";
        vector<Stones> pickup;
        vector<int> drops;
        assert((int)board[x][y].size() >= h);
        for(int i = 4; i < (int)move.length(); ++i)
            drops.push_back(move[i] - '0');
        for(int i = 0; i < h; ++i) {
            Stones stone = board[x][y].back();
            board[x][y].pop_back();
            pickup.push_back(stone);
        }
        for(int i = 0; i < (int)drops.size(); ++i) {
            x = next_x(x, dir);
            y = next_y(y, dir);
            assert(not out_of_bounds(x, y));
            for(int j = 0; j < drops[i]; ++j) {
                Stones stone = pickup.back();
                pickup.pop_back();
                if(board[x][y].empty() == false) {
                    if(board[x][y].back() == WHITE_WALL or board[x][y].back() == BLACK_WALL) {
                        crush = true;
                        cerr << "crushed at " << x << ", " << y << "\n";
                        assert(stone == WHITE_CAP or stone == BLACK_CAP);
                        board[x][y].back() = (board[x][y].back() == WHITE_WALL)
                                            ? WHITE_CRUSH : BLACK_CRUSH;
                    }
                }
                board[x][y].push_back(stone);
            }
        }
        return crush;
    }

    void undo_placement(Move move, bool white) {
        const pair<int, int> xy = make_xy(move[1], move[2]);
        const int x = xy.first; 
        const int y = xy.second;
        assert(board[x][y].size() == 1);
        assert(this->white(x, y) == white);
        board[x][y].pop_back();
    }

    void undo_motion(Move move, bool white, bool un_crush) {
        /*  read the height of the stack */
        const char dir = (move[3]);
        /* read the final coordinates */
        const pair<int, int> xy = make_xy(move[1], move[2]);
        const int x0 = xy.first;
        const int y0 = xy.second;
        int x = xy.first;
        int y = xy.second;
        vector<Stones> temp;
        vector<int> drops;
        for(int i = 4; i < (int)move.length(); ++i) {
            drops.push_back(move[i] - '0');
        }
        for(int i = 0; i < (int)drops.size(); ++i) {
            x = next_x(x, dir), y = next_y(y, dir);
            for(int j = 0; j < drops[i]; ++j) {
                Stones stone = board[x][y].back();
                board[x][y].pop_back();
                temp.push_back(stone);
            }
            assert(drops[i] == (int)temp.size());
            for(int j = 0; j < drops[i]; ++j) {
                Stones stone = temp.back();
                temp.pop_back();
                board[x0][y0].push_back(stone);
            }
        }
        if(un_crush == true) {
            assert(board[x][y].back() == BLACK_CRUSH or board[x][y].back() == WHITE_CRUSH);
            assert(board[x0][y0].back() == BLACK_CAP or board[x0][y0].back() == WHITE_CAP);
            cerr << "uncrushed at x = " << x << ", y = " << y << "\n";
            board[x][y].back() = (board[x][y].back() == BLACK_CRUSH) ? BLACK_WALL : WHITE_WALL;
        }
        assert(this->white(x0, y0) == white);
    }
};


void print_board(Board &board) {
    for(int j = N - 1; j >= 0; --j) {
        for(int i = 0; i < N; ++i) {
            cout << "[";
            for(auto stone : board.board[i][j]) {
                switch (stone) {
                    case WHITE_CAP: cout << "WC"; break;
                    case WHITE_WALL: cout << "WS"; break;
                    case WHITE_FLAT: cout << "w"; break;
                    case WHITE_CRUSH: cout << "ws"; break;

                    case BLACK_CAP: cout << "BC"; break;
                    case BLACK_WALL: cout << "BS"; break;
                    case BLACK_FLAT: cout << "b"; break;
                    case BLACK_CRUSH: cout << "bs"; break;
                }
                cout << ",";
            }
            cout << "]\t";
        }
        cout << "\n";
    }
}
void generate_moves(Board &board, bool white);
void generate_placement_moves(Board &board, Moves &moves, bool white);
void generate_motion_moves(Board &board, Moves &moves, bool white);
void motion(const char dir, int x, int y, string prefix, Board &board, Moves &moves, int ht);
void cap_motion(const char dir, int x, int y, string prefix, Board &board, Moves &moves, int ht);

void generate_moves(Board &board, Moves &moves, bool white) {
    /* generates a list of moves for either player and prints them out */
    // generate_placement_moves(board, moves, white);
    generate_motion_moves(board, moves, white);
    print_moves(moves);
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
                    moves.push_back("S" + s);
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

bool perform_move(Board &board, const Move move, bool white=true) {
    if(move[0] == 'F' || move[0] == 'S' || move[0] == 'C') 
        return board.perform_placement(move, white);
    else 
        return board.perform_motion(move, white);
}


int main() {
    vector<Move> moves;
    Board board;
    // board.board[0][0].push_back(BLACK_FLAT);
    // board.board[0][0].push_back(WHITE_CRUSH);
    // board.board[0][0].push_back(BLACK_CAP);
    // print_board(board);
    // Move undo_move = "3a1+21";
    // bool did_crush = perform_move(board, undo_move, false);
    // print_board(board);


    board.board[0][0].push_back(BLACK_FLAT);
    board.board[0][0].push_back(WHITE_CRUSH);
    board.board[0][0].push_back(BLACK_CAP);

    board.board[0][1].push_back(BLACK_CRUSH);
    board.board[0][2].push_back(WHITE_FLAT);
    board.board[0][3].push_back(BLACK_WALL);

    print_board(board);
    // generate_moves(board, moves, false);
    Move undo_move = "3a1+111";
    bool did_crush = perform_move(board, undo_move, false);
    cout << "did_crush  = " << did_crush << "\n";
    print_board(board);
    board.undo_motion(undo_move, false, did_crush);
    print_board(board);
    /*
    board.board[0][1].push_back(WHITE_WALL);
    board.board[0][0].push_back(BLACK_CAP);
    print_board(board);
    
    generate_moves(board, moves, false);
    Move move_to_undo = moves[0];
    moves.clear();
    cerr << "Move = " << move_to_undo << "\n";
    
    bool result = perform_move(board, move_to_undo, false);
    print_board(board);
    cout << "result = " << result << "\n";
    
    board.undo_motion(move_to_undo, false, result);
    print_board(board);
    */


    // generate_moves(board, moves, false);
}