#include "utility.h"
#include <iostream>
#include <cstring>
#include <cassert>
using namespace std;

typedef uint8_t int8;
typedef uint16_t int16;
typedef uint32_t int32;
typedef uint64_t int64;

typedef int8 Point;


const int N = 5;
class Board {
    /* searches for a road win by `player color` */

    int evaluate_captives(const bool player_color) const;
    int evaluate_tops(const bool player_color) const;

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
    int evaluate_helper(bool player_color);

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

    // Utilities to calculate hash of positions

    const int64 fnvBasis = 14695981039346656037;
    const int64 fnvPrime = 1099511628211;
    // TODO: initialize values for 5x5 basis array
    int64 basis[5][5];

    /* ------------------------------------------- */

    void initBasis() {
      basis[0][0] = 18183439681736624571;
      basis[0][1] = 6027321677903241570;
      basis[0][2] = 7291301521048381893;
      basis[0][3] = 16707003755830242952;
      basis[0][4] = 10256755329810482736;
      basis[1][0] = 6343378603570537650;
      basis[1][1] = 6662624611957088715;
      basis[1][2] = 11812679395820742279;
      basis[1][3] = 12623970433846802091;
      basis[1][4] = 355866642316429502;
      basis[2][0] = 9214262323919156496;
      basis[2][1] = 2687856525294241330;
      basis[2][2] = 7372598617540182037;
      basis[2][3] = 13345869983351199968;
      basis[2][4] = 7648745695164505374;
      basis[3][0] = 15308890490440544787;
      basis[3][1] = 722985701341625725;
      basis[3][2] = 52572759407397074;
      basis[3][3] = 4647247616217886398;
      basis[3][4] = 12286721093444641706;
      basis[4][0] = 13362207002028843134;
      basis[4][1] = 18060721682244888260;
      basis[4][2] = 11725222004378576229;
      basis[4][3] = 13225017917370162217;
      basis[4][4] = 11877932724692211758;
    }

    // hash function for board: inspired by takticianbot
    int64 getHash(bool toMove) {
      int64 whiteControl = 0;
      int64 blackControl = 0;
      int64 capstone = 0;
      int64 standing = 0;
      int64 h = fnvBasis;
      for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
           int index = i * N + j;
           if(white(i,j)) {
             whiteControl |= (1<<index);
           }
           if(black(i,j)) {
             blackControl |= (1<<index);
           }
           if(caps(i,j)) {
             capstone |= (1<<index);
           }
           if(wall(i,j)) {
             standing |= (1<<index);
           }
           h ^= hashAt(i,j);
        }
      }
      h = hash64(h, whiteControl);
      h = hash64(h, blackControl);
      h = hash64(h,standing);
      h = hash64(h, capstone);
      h = hash8(h, (int64) toMove);
      return h;
    }

    // get hash for pieces at position (i,j) of board
    int64 hashAt(int x, int y) {
      if(height(x,y) == 0) {
        return 0;
      }
      else {
        int64 stackheight = (int64)height(x,y);
        int64 stackbits = getStackBit64(x, y);
        return hash64(hash8(basis[x][y], stackheight), stackbits);
      }
    }

    int64 getStackBit64(int x, int y) {
      int64 stackbits = 0;
      vector<Stones> stack = board[x][y];
      // Note the stack.size()-1 in loop:
      for (int i = 0; i < (int)stack.size()-1; i++) {
        // 1 for black piece, 0 for white
        if(stack[i] == BLACK_FLAT || stack[i] == BLACK_WALL || stack[i] == BLACK_CAP || stack[i] == BLACK_CRUSH) {
          stackbits |= (1<<i);
        }
      }
      return stackbits;
    }

    int64 hash8(int64 basis, int64 b) {
      return (basis ^ b) * fnvPrime;
    }

    int64 hash64(int64 basis, int64 w) {
      int h = basis;
    	h = (h ^ (w & 0xff)) * fnvPrime;
    	h = (h ^ ((w >> 8) & 0xff)) * fnvPrime;
    	h = (h ^ ((w >> 16) & 0xff)) * fnvPrime;
    	h = (h ^ (w >> 24)) * fnvPrime;
    	return h;
    }

    // End of hash utilities

};
