/*
 * This code is to evaluate a bitboard
 * This is functional more than object oriented
 * bitboard_t = int64
 */
#include <cstdint>
#include "bitboard.h"


// const uint64_t MAX_EVAL = 1 << 30;
// const uint64_t MIN_EVAL = -(1 << 30);
const int ENDGAME_CUTOFF = 7;

struct Weights {
    int TopFlat;
    int EndGameFlat;
    int Standing;
    int Capstone;

    int Groups[8];
    int GroupLiberties;

    int Center;

    Weights() {
        TopFlat = 100;
        EndGameFlat = 10;
    }
};

inline bitboard_t make_bitboard(const Board &board) {
    /* TODO: test this function */
    assert(board.white_flats_rem >= 0);
    bitboard_t bitboard;
    return bitboard;
}
//
// inline bool check_gameover(uint64_t bitboard) {
//     assert(bitboard >= 0);
//     return false;
// }
//
// inline uint64_t evaluate_terminal(const Board &board, bool winner, bool player_color) {
//     /* TODO */
//     // const int move_scale = 100;
//     // uint64_t pieces = 0;
//     uint64_t return_value = 1;
//     assert(board.white_flats_rem >= 0);
//     return (return_value << 32) + winner + player_color;
// }

// uint64_t evaluate_winner(const const_bitboard c) {
//     /* TODO: recheck signature */
// 	// if over, winner := p.GameOver(); over {
// 	// 	return evaluate_terminal(p, winner);
// 	// }
// 	return 0;
// }

uint64_t evaluate(const Board &board, const Weights &w, const const_bitboard &c, bool player_color) {
    /* player color is the color of the player who has to make a move next */
    bitboard_t bb = make_bitboard(board);

    /* Handle terminal business */
    uint64_t score = 0;
    int left = min(bb.white_stones, bb.black_stones);
    left = min(left, ENDGAME_CUTOFF);
    uint64_t flat = w.TopFlat * ((ENDGAME_CUTOFF - left) * w.EndGameFlat / ENDGAME_CUTOFF);
    /* calculate the score based on the color */
    /* white has an advantage */
    if(player_color)    score += (flat / 2) + 50;
    else                score -= (flat / 2) + 50;

    /* scoring the naive board position */
    score += int64(popcount(bb.white&(~(bb.caps|bb.standing))) * flat);
	score -= int64(popcount(bb.black&(~(bb.caps|bb.standing))) * flat);
	score += int64(popcount(bb.white&bb.standing) * w.Standing);
	score -= int64(popcount(bb.black&bb.standing) * w.Standing);
	score += int64(popcount(bb.white&bb.caps) * w.Capstone);
	score -= int64(popcount(bb.black&bb.caps) * w.Capstone);

    /* scoring centre control */
    score += int64(popcount(bb.white&~c.edge) * w.Center);
    score -= int64(popcount(bb.black&~c.edge) * w.Center);

    return score;
}

uint64_t score_groups(const const_bitboard &c, const Weights ws, vector<uint64_t> gs, uint64_t other) {
    /* scores the groups */
    uint64_t allg = 0;
    uint64_t sc = 0;
    for(uint64_t &g : gs) {
        pair<int, int> wh = dimensions(c, g);
        int w = wh.first, h = wh.second;
        sc += ws.Groups[w];
        sc += ws.Groups[h];
        allg |= g;
    }
	if (ws.GroupLiberties != 0) {
		int libs = popcount(grow(c, ~other, allg) & (~allg));
		sc += libs * ws.GroupLiberties;
	}

	return sc;
}


int main() {
    /* code */
    return 0;
}
