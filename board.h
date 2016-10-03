#include "utility.h"
#include <cassert>
#include <iostream>
using namespace std;


const int N = 5;
class Board {
    int evaluate_captives(bool player_color);
    int evaluate_tops(bool player_color);

    /* perform the two types of moves */
    bool perform_placement(Move move, bool white);
    bool perform_motion(Move move, bool white);

    /* undo the two types of moves */
    void undo_placement(Move move, bool white);
    void undo_motion(Move move, bool white, bool uncrush);
public:
    /* for debugging purposes, they are outside */
    vector<Stones> board[5][5];
    int white_flats_rem = 21;
    int white_caps_rem = 1;
    int black_flats_rem = 21;
    int black_caps_rem = 1;

    /* move on the board, returns if it did crush a wall */
    bool perform_move(const Move &move, bool white);
    /* undo the above move */
    void undo_move(const Move &move, bool white, bool uncrush);

    /* evaluates the move */
    int evaluate(bool player_color);

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
    bool white_crush(int x, int y) {
        return (not empty(x, y) and board[x][y].back() == WHITE_CRUSH);
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
    bool black_crush(int x, int y) {
        return (not empty(x, y) and board[x][y].back() == BLACK_CRUSH);
    }

    bool white(int x, int y) {
        return white_flat(x, y) || white_wall(x, y) || white_cap(x, y) || white_crush(x, y);
    }
    bool black(int x, int y) {
        return black_flat(x, y) || black_wall(x, y) || black_cap(x, y) || black_crush(x, y);
    }
    bool flat(int x, int y) {
      return white_flat(x, y) || black_flat(x, y);
    }
    bool caps(int x, int y) {
      return white_cap(x, y) || black_cap(x, y);
    }
    bool wall(int x, int y) {
      return white_wall(x, y) || black_wall(x, y);
    }
    bool crush(int x, int y) {
      return white_crush(x, y) || black_crush(x, y);
    }
    int height(int x, int y) {
        return board[x][y].size();
    }
};
