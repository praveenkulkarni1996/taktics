/*
 * This code is to evaluate a bitboard
 * This is functional more than object oriented
 * bitboard_t = int64
 */
#include "evaluate.h"

// const uint64_t MAX_EVAL = 1 << 30;
// const uint64_t MIN_EVAL = -(1 << 30);
const int ENDGAME_CUTOFF = 7;

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
/* divides them into groups */




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
    score += popcount(bb.white&(~(bb.caps|bb.standing))) * flat;
	score -= popcount(bb.black&(~(bb.caps|bb.standing))) * flat;
	score += popcount(bb.white&bb.standing) * w.Standing;
	score -= popcount(bb.black&bb.standing) * w.Standing;
	score += popcount(bb.white&bb.caps) * w.Capstone;
	score -= popcount(bb.black&bb.caps) * w.Capstone;

    /* scoring centre control */
    score += popcount(bb.white&~c.edge) * w.Center;
    score -= popcount(bb.black&~c.edge) * w.Center;

    /* scoring groups */
    vector<uint64_t> white_groups, black_groups;
    analyze(c, bb, white_groups, black_groups);
    score += score_groups(c, white_groups, w, bb.black|bb.standing);
	score -= score_groups(c, black_groups, w, bb.white|bb.standing);

    if (w.Liberties != 0) {
        uint64_t wr = bb.white&~bb.standing;
        uint64_t br = bb.black&~bb.standing;
        uint64_t wl = popcount(grow(c, ~bb.black, wr) & (~bb.white));
        uint64_t bl = popcount(grow(c, ~bb.white, br) & (~bb.black));
        score += w.Liberties * wl;
        score -= w.Liberties * bl;
    }

    // score += score_threats(c, w, bb);
    // score += score_control(c, w, bb);

    return (player_color) ? score : -score;
}

/* scores the groups */
uint64_t score_groups(const const_bitboard &c,
        const vector<uint64_t> &gs,
        const Weights &ws,
        uint64_t other) {
    uint64_t allg = 0;
    uint64_t sc = 0;
    for(const uint64_t &g : gs) {
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
