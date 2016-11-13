#include <cstdint>
#include "bitboard.h"

struct Weights {
    int TopFlat;
    int EndGameFlat;
    int Standing;
    int Capstone;

    int Groups[8];
    int GroupLiberties;
    int Liberties;

    int Center;

    Weights() {
        TopFlat = 100;
        EndGameFlat = 10;
    }
};

inline bitboard_t make_bitboard(const Board &board) {
    /* TODO: test this function */
    const int N = 5;
    // top to bottom, left to right
    bitboard_t bitboard;
    for(int y=N-1; y >= 0; --y) {
        for(int x = 0; x < N; ++x) {
            bitboard.white <<= 1;
            bitboard.black <<= 1;
            bitboard.standing <<= 1;
            bitboard.caps <<= 1;
            if(board.white(x, y)) bitboard.white |= 1;
            if(board.black(x, y)) bitboard.black |= 1;
            if(board.white_cap(x, y) || board.black_cap(x, y))
                bitboard.caps |= 1;
            if(board.white_wall(x, y) || board.black_wall(x, y))
                bitboard.standing |= 1;
        }
    }
    /* TODO: this may be incorrect */
    bitboard.white_stones = board.white_flats_rem;
    bitboard.black_stones = board.black_flats_rem;
    return bitboard;
}

inline void print_bitmask(uint64_t mask) {
    int cnt = (N * N) - 1;
    for(int y=N-1; y >=0; --y) {
        for(int x = 0; x < N; ++x) {
            printf("%llu", 1 & (mask >> cnt--));
        }   printf("\n");
    }
}

inline void print_bitboard(bitboard_t &bitboard) {
    printf("white:\n"); print_bitmask(bitboard.white);
    printf("black:\n"); print_bitmask(bitboard.black);
    printf("capstone:\n"); print_bitmask(bitboard.caps);
    printf("standing:\n"); print_bitmask(bitboard.standing);
}

uint64_t score_groups(const const_bitboard &c,
        const vector<uint64_t> &gs,
        const Weights &ws,
        uint64_t other);
