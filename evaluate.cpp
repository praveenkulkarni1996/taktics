/*
 * This code is to evaluate a bitboard
 * This is functional more than object oriented
 * bitboard_t = int64
 */
#include "evaluate.h"


// const uint64_t MAX_EVAL = 1 << 30;
// const uint64_t MIN_EVAL = -(1 << 30);

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
