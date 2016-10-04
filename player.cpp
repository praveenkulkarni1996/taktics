#include <iostream>
#include <vector>
#include <string>
#include <cassert>
// #include "utility.h"
#include "board.h"

using namespace std;

typedef uint8_t int8;
typedef uint16_t int16;
typedef uint32_t int32;
typedef uint64_t int64;

typedef int8 Point;

void print_board(const Board &board) {
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
// void generate_moves(const Board &board, const bool white);
void generate_placement_moves(const Board &board, Moves &moves, const bool white);
void generate_motion_moves(const Board &board, Moves &moves, const bool white);
void motion(const char dir, const int x, const int y, string prefix, const Board &board, Moves &moves, const int ht);
void cap_motion(const char dir, const int x, const int y, string prefix, const Board &board, Moves &moves, const int ht);
/* declaration of the new functions */
int alpha_beta_search(Board &board, const int cutoff, const bool player_color);
int max_value(Board &board, int alpha, int beta, const int cutoff, const bool player_color);
int min_value(Board &board, int alpha, int beta, const int cutoff, const bool player_color);

//=========================================================

void generate_moves(const Board &board, Moves &moves, const bool white) {
    /* generates a list of moves for either player and prints them out */
    generate_placement_moves(board, moves, white);
    generate_motion_moves(board, moves, white);
    // print_moves(moves);
}

void generate_placement_moves(const Board &board, Moves &moves, const bool white) {
    /* generates moves to place pieces for white and black */
    const int flats = (white) ? board.white_flats_rem : board.black_flats_rem;
    const int caps = (white) ? board.white_caps_rem : board.black_caps_rem;
    for(int x = 0; x < N; ++x) {
        for(int y = 0; y < N; ++y) {
            if(board.empty(x, y)) {
                string s = make_sqr(x, y);
                if(flats > 0) {
                    moves.push_back((const string)("S" + s));
                    moves.push_back((const string)("F" + s));
                }
                if(caps > 0) {
                    moves.push_back("C" + s);
                }
            }
        }
    }
}

void generate_motion_moves(const Board &board, Moves &moves, const bool white) {
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

void motion(const char dir, const int x, const int y, string prefix, const Board &board, Moves &moves, const int ht) {
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

void cap_motion(const char dir, const int x, const int y, string prefix, const Board &board, Moves &moves, const int ht) {
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

// TODO: game-over
int alpha_beta_search(Board &board, const int cutoff, const bool player_color) {
     return max_value(board, INT_MIN, INT_MAX, cutoff, player_color);
}

int max_value(Board &board, int alpha, int beta, const int cutoff, const bool player_color) {
    /* has the other player won the game ? */
    if(board.game_over(not player_color)) return INT_MIN;
    if(cutoff == 0) return board.evaluate(player_color);

    vector<Move> moves;
    int value = INT_MIN;
    generate_moves(board, moves, player_color);
    // TODO: sort by evalutation function
    // TODO: run checks if undo works correctly
    for(const auto &move : moves) {
        const bool did_crush = board.perform_move(move, player_color);
        value = max(value, min_value(board, alpha, beta, cutoff-1, not player_color));
        board.undo_move(move, player_color, did_crush);
        if(value >= beta) return value;
        alpha = max(alpha, value);
    }
    return value;
}

int min_value(Board &board, int alpha, int beta, const int cutoff, const bool player_color) {
    /* has the other player won the game ? */
    if(board.game_over(not player_color)) return INT_MIN;
    if(cutoff == 0) return board.evaluate(player_color);
    int value = INT_MAX;
    vector<Move> moves;
    generate_moves(board, moves, player_color);
    // same TODOS
    for(const auto &move : moves) {
        const bool did_crush = board.perform_move(move, player_color);
        value = min(value, max_value(board, alpha, beta, cutoff-1, not player_color));
        board.undo_move(move, player_color, did_crush);
        if(value <= alpha) return value;
        beta = min(beta, value);
    }
    return value;
}

int main() {
    vector<Move> moves;
    Board board;

    board.board[0][0].push_back(BLACK_FLAT);
    board.board[0][0].push_back(WHITE_CRUSH);
    board.board[0][0].push_back(BLACK_CAP);

    board.board[0][1].push_back(BLACK_CRUSH);
    board.board[0][2].push_back(WHITE_FLAT);
    board.board[0][3].push_back(BLACK_CRUSH);

    print_board(board);
    int value = alpha_beta_search(board, 6, false);

    cerr << "value = " << value << "\n";
    print_board(board);

    // Move undo_move = "3a1+111";
    // bool did_crush = board.perform_move(undo_move, false);
    // cout << "did_crush  = " << did_crush << "\n";
    // print_board(board);
    // int black_value = board.evaluate(false);
    // int white_value = board.evaluate(true);
    // print_board(board);  

    // cerr << "evaluation = " <<  white_value << " : " << black_value << "\n";
    // board.undo_move(undo_move, false, did_crush);
    // cerr << "\n";
    // print_board(board);


    // board.board[0][0].push_back(WHITE_CRUSH);
    // board.board[0][0].push_back(BLACK_CAP);
    // board.board[0][2].push_back(WHITE_CRUSH);
    // board.board[0][2].push_back(WHITE_WALL);
    // generate_moves(board, moves, false);
    // print_board(board);
    // const Move move = "2a1+11";
    // bool didcrush = board.perform_move(move, false);
    // print_board(board);
    // board.undo_move(move, false, didcrush);
    // print_board(board);

    // board.board[0][1].push_back(WHITE_WALL);
    // board.board[0][0].push_back(BLACK_CAP);
    // print_board(board);

    // generate_moves(board, moves, false);
    // Move move_to_undo = moves[0];
    // moves.clear();
    // cerr << "Move = " << move_to_undo << "\n";

    // bool result = board.perform_move(move_to_undo, false);
    // print_board(board);
    // cout << "result = " << result << "\n";

    // board.undo_motion(move_to_undo, false, result);
    // print_board(board);


    // generate_moves(board, moves, false);
}
