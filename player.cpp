#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <climits>
#include <cassert>
// #include "utility.h"
#include "board.h"

using namespace std;

void print_board(const Board &board) {
    for(int j = N - 1; j >= 0; --j) {
        for(int i = 0; i < N; ++i) {
            cerr << "[";
            for(auto stone : board.board[i][j]) {
                switch (stone) {
                    case WHITE_CAP: cerr << "WC"; break;
                    case WHITE_WALL: cerr << "WS"; break;
                    case WHITE_FLAT: cerr << "w"; break;
                    case WHITE_CRUSH: cerr << "ws"; break;

                    case BLACK_CAP: cerr << "BC"; break;
                    case BLACK_WALL: cerr << "BS"; break;
                    case BLACK_FLAT: cerr << "b"; break;
                    case BLACK_CRUSH: cerr << "bs"; break;
                }
                cerr << ",";
            }
            cerr << "]\t";
        }
        cerr << "\n";
    }
}
// void generate_moves(const Board &board, const bool white);
void generate_placement_moves(const Board &board, Moves &moves, const bool white);
void generate_motion_moves(const Board &board, Moves &moves, const bool white);
void motion(const char dir, const int x, const int y, string prefix, const Board &board, Moves &moves, const int ht);
void cap_motion(const char dir, const int x, const int y, string prefix, const Board &board, Moves &moves, const int ht);
/* declaration of the new functions */
pair<Move, int> alpha_beta_search(Board &board, const int cutoff, const bool player_color);
pair<Move, int> max_value(Board &board, int alpha, int beta, const int cutoff, const bool player_color);
pair<Move, int> min_value(Board &board, int alpha, int beta, const int cutoff, const bool player_color);

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
pair<Move, int> alpha_beta_search(Board &board, const int cutoff, const bool player_color) {
     return max_value(board, INT_MIN, INT_MAX, cutoff, player_color);
}

pair<Move, int> max_value(Board &board, int alpha, int beta, const int cutoff, const bool player_color) {
    /* has the other player won the game ? */
    if(board.game_over(not player_color)) return make_pair("", INT_MIN);
    if(cutoff == 0) return make_pair("", board.evaluate(player_color));

    int value = INT_MIN;
    Move optimal_move;
    vector<Move> moves;
    generate_moves(board, moves, player_color);
    // TODO: sort by evalutation function
    // TODO: run checks if undo works correctly
    for(const auto &move : moves) {
        const bool did_crush = board.perform_move(move, player_color);
        // value = max(value, min_value(board, alpha, beta, cutoff-1, not player_color));
        int move_min_value = min_value(board, alpha, beta, cutoff-1, player_color).second;
        // cerr << "MAX-> " << "Move: " << move << "\t" << "Value: " << move_min_value << endl;
        if(value < move_min_value) {
          value = move_min_value;
          optimal_move = move;
        }

        board.undo_move(move, player_color, did_crush);
        if(value >= beta) return make_pair(move, value);
        alpha = max(alpha, value);
    }
    pair<Move, int> move_pair = make_pair(optimal_move, value);
    return move_pair;
}

pair<Move, int> min_value(Board &board, int alpha, int beta, const int cutoff, const bool player_color) {
    /* has the other player won the game ? */
    if(board.game_over(not player_color)) return make_pair("", INT_MIN);
    if(cutoff == 0) return make_pair("", board.evaluate(player_color));
    int value = INT_MAX;
    Move optimal_move;
    vector<Move> moves;
    generate_moves(board, moves, player_color);
    // same TODOS
    for(const auto &move : moves) {
        const bool did_crush = board.perform_move(move, player_color);
        // value = min(value, max_value(board, alpha, beta, cutoff-1, not player_color));
        int move_max_value = max_value(board, alpha, beta, cutoff-1, player_color).second;
        // cerr << "MIN-> " << "Move: " << move << "\t" << "Value: " << move_min_value << endl;
        if(value > move_max_value) {
          value = move_max_value;
          optimal_move = move;
        }
        board.undo_move(move, player_color, did_crush);
        if(value <= alpha) return make_pair(move, value);
        beta = min(beta, value);
    }
    pair<Move, int> move_pair = make_pair(optimal_move, value);
    return move_pair;
}

int main() {
    vector<Move> moves;
    Board board;
    board.initBasis();

    // bool player_color = true; // White chosen

    // board.board[0][0].push_back(BLACK_FLAT);
    // board.board[0][0].push_back(WHITE_CRUSH);
    // board.board[0][0].push_back(BLACK_CAP);
    //
    // board.board[0][1].push_back(BLACK_CRUSH);
    // board.board[0][2].push_back(WHITE_FLAT);
    // board.board[0][3].push_back(BLACK_CRUSH);

    // board.board[0][0].push_back(WHITE_FLAT);
    // board.board[4][4].push_back(BLACK_FLAT);
    //
    // int depth = 1;
    // for(int i = 0; i < 2; ++i) {
    //   pair<Move, int> optimal_move = alpha_beta_search(board, depth, player_color);
    //   cerr << "Move: " << optimal_move.first << endl;
    //   cerr << "Value: " << optimal_move.second << endl;
    //   board.perform_move(optimal_move.first, player_color);
    //   cout << "White flats remaining: " << board.white_flats_rem << endl;
    //   cout << "White caps remaining: " << board.white_caps_rem << endl;
    //   cout << "Black flats remaining: " << board.black_flats_rem << endl;
    //   cout << "Black caps remaining: " << board.black_caps_rem << endl;
    //   print_board(board);
    // }

    int player_number;
    int board_size;
    int time_limit;
    cin >> player_number >> board_size >> time_limit;
    bool player_color = (player_number == 1);
    print_board(board);
    string first_move;
    if (player_color) {
      // First move - hardcoded to Fa2
      first_move = "Fa2";
    }
    else {
      // Listen and then print
      assert(false);
    }
    board.perform_move((Move)first_move, not player_color);  // Reverse piece moved in first move
    cout<< first_move << "\n" << flush;
    print_board(board);

    string my_move;
    string opponent_move;

    cin >> opponent_move;
    cerr << "Opponent moved: *" << opponent_move << "*" << endl;
    board.perform_move((Move)opponent_move, player_color);  // Reverse piece moved in first move
    print_board(board); // Print board after opponent's move

    // Main game
    int depth = 5;
    while (true) {
      // Your move
      pair<Move, int> move_pair = alpha_beta_search(board, depth, player_color);
      my_move = move_pair.first;
      board.perform_move((Move)my_move, player_color);
      cerr << "value = " << move_pair.second << "\n";
      cout << my_move << "\n" << flush;
      print_board(board); // Print board after your move
      // if(board.game_over(not player_color)) {
      //   break;  // You win
      // }

      // Opponent's move
      cin >> opponent_move;
      cerr << "Opponent moved: *" << opponent_move << "*" << endl;
      board.perform_move((Move)opponent_move, not player_color);
      print_board(board); // Print board after opponent's move
      // if(board.game_over(player_color)) {
      //   break;  // You lose
      // }
    }
}
