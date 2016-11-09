#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <cassert>
#include <ctime>
#include "board.h"
#include <unordered_map>

using namespace std;

typedef unordered_map<string, pair<pair<Move, int>, int> > tranposition_table;
tranposition_table max_table;
tranposition_table min_table;

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
void generate_placement_moves(const Board &board, Moves &moves, const bool white);
void generate_motion_moves(const Board &board, Moves &moves, const bool white);
void motion(const char dir, const int x, const int y, string prefix, const Board &board, Moves &moves, const int ht);
void cap_motion(const char dir, const int x, const int y, string prefix, const Board &board, Moves &moves, const int ht);
/* declaration of the new functions */
const pair<Move, int> alpha_beta_search(Board &board, const int cutoff, const bool player_color);
const pair<Move, int> max_value(Board &board, int alpha, int beta, const int cutoff, const bool player_color);
const pair<Move, int> min_value(Board &board, int alpha, int beta, const int cutoff, const bool player_color);

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

const pair<Move, int> alpha_beta_search(Board &board, const int cutoff, const bool player_color) {
    return max_value(board, INT_MIN, INT_MAX, cutoff, player_color);
}


const pair<Move, int> max_value(Board &board, int alpha, int beta, const int cutoff, const bool player_color) {
    /* memoized in the hash table */
    const string hash_string = board.board_to_string();
    if(max_table.count(hash_string) == 1) {
        auto memo = max_table[hash_string];
        if(memo.second >= cutoff) {
            return memo.first;
        }
    }
    /* has the other player won the game ? */
    if(board.player_road_win(not player_color)) return make_pair("", INT_MIN);
    if(board.player_road_win(player_color)) return make_pair("", INT_MAX);
    if(board.game_flat_win()) {
        const bool player_win = board.player_flat_win(player_color);
        const bool other_win = board.player_flat_win(not player_color);
        if(player_win) return make_pair("", INT_MAX);
        if(other_win) return make_pair("", INT_MIN);
    }
    if(cutoff <= 0) return make_pair("", board.evaluate(player_color));

    /* iterative deepening code */
    int value = INT_MIN;
    Move optimal_move;
    vector<Move> moves;
    generate_moves(board, moves, player_color);
    vector<pair<int, Move> > order;
    for(const auto &move : moves) {
        const bool did_crush = board.perform_move(move, player_color);
        const int value = min_value(board, alpha, beta, cutoff-3, player_color).second;
        board.undo_move(move, player_color, did_crush);
        order.push_back(make_pair(value, move));
    }
    /* sorts them according to order */
    sort(order.rbegin(), order.rend());
    /* main alpha beta code */
    for(const auto &order_pair : order) {
        const auto move = order_pair.second;
        const bool did_crush = board.perform_move(move, player_color);
        int move_min_value = min_value(board, alpha, beta, cutoff-1, player_color).second;
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

const pair<Move, int> min_value(Board &board, int alpha, int beta, const int cutoff, const bool player_color) {
    /* memoized in the hash table */
    const string hash_string = board.board_to_string();
    if(min_table.count(hash_string) == 1) {
        auto memo = min_table[hash_string];
        if(memo.second >= cutoff) {
            return memo.first;
        }
    }
    /* has the other player won the game */
    if(board.player_road_win(player_color)) return make_pair("", INT_MAX);
    if(board.player_road_win(not player_color)) return make_pair("", INT_MIN);
    if(board.game_flat_win()) {
        const bool player_win = board.player_flat_win(player_color);
        const bool other_win = board.player_flat_win(not player_color);
        if(player_win) return make_pair("", INT_MAX);
        if(other_win) return make_pair("", INT_MIN);
    }
    if(cutoff <= 0) return make_pair("", board.evaluate(player_color));
    /* iterative deepening code*/
    int value = INT_MAX;
    Move optimal_move;
    vector<Move> moves;
    generate_moves(board, moves, not player_color);
    vector<pair<int, Move> > order;
    for(const auto &move : moves) {
        const bool did_crush = board.perform_move(move, not player_color);
        const int value = max_value(board, alpha, beta, cutoff-3, player_color).second;
        board.undo_move(move, not player_color, did_crush);
        order.push_back(make_pair(value, move));
    }
    sort(order.begin(), order.end());
    /* main alpha beta */
    for(const auto &order_pair : order) {
        const auto move = order_pair.second;
        const bool did_crush = board.perform_move(move, not player_color);
        int move_max_value = max_value(board, alpha, beta, cutoff-1, player_color).second;
        if(value > move_max_value) {
          value = move_max_value;
          optimal_move = move;
        }
        board.undo_move(move, not player_color, did_crush);
        if(value <= alpha) return make_pair(move, value);
        beta = min(beta, value);
    }
    // cerr << "MIN choice = " << optimal_move << " , " << value << "\n";
    pair<Move, int> move_pair = make_pair(optimal_move, value);
    min_table.insert(make_pair(hash_string, make_pair(move_pair, cutoff)));
    return move_pair;
}




int main() {
     // testing
    //  vector<Move> moves;
    //  Board board;

    //  board.board[2][0].push_back(WHITE_FLAT);
    //  board.board[2][1].push_back(WHITE_FLAT);
    //  board.board[2][2].push_back(WHITE_FLAT);
    //  // board.board[3][2].push_back(WHITE_FLAT);
    //  // board.board[3][3].push_back(WHITE_FLAT);
    //  board.board[2][4].push_back(WHITE_FLAT);

    // //  board.board[2][2].push_back(BLACK_FLAT);

    // cerr << board.evaluate_components(true) << endl;
    // cerr << board.evaluate(true) << endl;
    // print_board(board);


    //  auto result = alpha_beta_search(board, 3, false);
    //  cerr << "Move: " << result.first <<endl;
    //  cerr << "value: " << result.second << endl;

     // end testing

    // -----------------------------------------------------------------

   vector<Move> moves;
   Board board;
   min_table.reserve((int)1e8);
   max_table.reserve((int)1e8);
   int player_number;
   int board_size;
   int time_limit;
   string opponent_move;
   cin >> player_number >> board_size >> time_limit;
   int time_count = time_limit;
   bool player_color = (player_number == 1);

   string first_move = "Fa1";
   // First move
   if (player_color) {
       board.perform_move((Move)first_move, not player_color); // place opponent piece
       cout << first_move << "\n" << flush;
       // print_board(board);
       cin >> opponent_move;
       board.perform_move((Move)opponent_move, player_color); // opponent moves my piece
       // print_board(board); // Print board after opponent's move
       // cerr << "Opponent moved: *" << opponent_move << "*" << endl;
   }
   else {
       cin >> opponent_move;
       board.perform_move((Move)opponent_move, player_color);
       // cerr << "Opponent moved: *" << opponent_move << "*" << endl;
       if(opponent_move == first_move) first_move = "Fe1";
       board.perform_move((Move)first_move, not player_color);
       cout << first_move << "\n" << flush;
   }

   // Main game
   int depth = 5;
   int depth_constrained = 4;
   int depth_play;
   int time_constrained = 60;
   int elapsed_time;

   while (true) {
       if(time_count < time_constrained) {
           depth_play = depth_constrained;
       }
       else {
           depth_play = depth;
       }
       // cerr << depth_play << endl;

       if(player_color) {
           clock_t start_time = clock();
           const auto result = alpha_beta_search(board, depth_play, player_color);
           cout << result.first << "\n" << flush;
           board.perform_move(result.first, player_color);
           elapsed_time = (int)(double(clock()-start_time) / (double)CLOCKS_PER_SEC);
           time_count -= elapsed_time;
           cin >> opponent_move;
           // cerr << "Opponent moved: *" << opponent_move << "*" << endl;
           board.perform_move((Move)opponent_move, not player_color);
           // print_board(board); // Print board after opponent's move
       }
       else {
           cin >> opponent_move;
           // cerr << "Opponent moved: *" << opponent_move << "*" << endl;
           board.perform_move((Move)opponent_move, not player_color);
           // print_board(board); // Print board after opponent's move
           clock_t start_time = clock();
           const auto result = alpha_beta_search(board, depth_play, player_color);
           cout << result.first << "\n" << flush;
           board.perform_move(result.first, player_color);
           elapsed_time = (int)(double(clock()-start_time) / (double)CLOCKS_PER_SEC);
           time_count -= elapsed_time;
       }
   }
}
