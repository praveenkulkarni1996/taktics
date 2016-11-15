#include "utility.h"
#include <iostream>
#include <cstring>
#include <cassert>
using namespace std;


struct LRUD {
    int l, r, u, d;
};

const int N = 5;
class Board {
    /* searches for a road win by `player color` */

    int evaluate_captives(const bool player_color) const;
    int evaluate_tops(const bool player_color) const;
    int evaluate_central_control(const bool player_color) const;

    /* perform the two types of moves */
    bool perform_placement(Move move, bool white);
    bool perform_motion(Move move, bool white);

    /* undo the two types of moves */
    void undo_placement(Move move, bool white);
    void undo_motion(Move move, bool white, bool uncrush);
public:
    bool player_road_win(const bool player_color) const;
    bool player_flat_win(const bool player_color) const;
    bool game_flat_win() const;
    string board_to_string() const;
    /* for debugging purposes, they are outside */
    vector<Stones> board[N][N];
    int white_flats_rem = 21;
    int white_caps_rem = 1;
    int black_flats_rem = 21;
    int black_caps_rem = 1;

    /* move on the board, returns if it did crush a wall */
    bool perform_move(const Move &move, bool white);
    /* undo the above move */
    void undo_move(const Move &move, bool white, bool uncrush);

    /* evaluates the move */
    int evaluate(const bool player_color) const;
    int evaluate_helper(const bool player_color) const;

    bool empty(int x, int y) const {
        assert(0 <= x and x < N);
        assert(0 <= y and y < N);
        return board[x][y].empty();
    }

    bool road_win() const {
        /* this game over simply tells you if the game is over */
        /* it does not tell you who won */
        /* TODO: add flat win */
      return player_road_win(true) || player_road_win(false);
    }

    bool white_wall(int x, int y) const {
        return (not empty(x, y) and board[x][y].back() == WHITE_WALL);
    }
    bool white_flat(int x, int y) const {
        return (not empty(x, y) and board[x][y].back() == WHITE_FLAT);
    }
    bool white_cap(int x, int y) const {
        return (not empty(x, y) and board[x][y].back() == WHITE_CAP);
    }
    bool white_crush(int x, int y) const {
        return (not empty(x, y) and board[x][y].back() == WHITE_CRUSH);
    }
    bool black_wall(int x, int y) const {
        return (not empty(x, y) and board[x][y].back() == BLACK_WALL);
    }
    bool black_flat(int x, int y) const {
        return (not empty(x, y) and board[x][y].back() == BLACK_FLAT);
    }
    bool black_cap(int x, int y) const {
        return (not empty(x, y) and board[x][y].back() == BLACK_CAP);
    }
    bool black_crush(int x, int y) const {
        return (not empty(x, y) and board[x][y].back() == BLACK_CRUSH);
    }
    bool road_piece(int x, int y) const {
        return white_crush(x, y) || white_cap(x, y) || white_flat(x, y)
          || black_crush(x, y) || black_cap(x, y) || black_flat(x, y);
    }
    bool white(int x, int y) const {
        return white_flat(x, y) || white_wall(x, y) || white_cap(x, y) || white_crush(x, y);
    }
    bool black(int x, int y) const {
        return black_flat(x, y) || black_wall(x, y) || black_cap(x, y) || black_crush(x, y);
    }
    bool flat(int x, int y) const {
        return white_flat(x, y) || black_flat(x, y);
    }
    bool caps(int x, int y) const {
      return white_cap(x, y) || black_cap(x, y);
    }
    bool wall(int x, int y) const {
      return white_wall(x, y) || black_wall(x, y);
    }
    bool crush(int x, int y) const {
      return white_crush(x, y) || black_crush(x, y);
    }
    int height(int x, int y) const {
        return board[x][y].size();
    }
    void dfs(const int x, const int y, LRUD &lrud, const bool player_color) const;
    int evaluate_components(const bool player_color) const;
};
