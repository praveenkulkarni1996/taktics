#include <cstdint>
#include "bitboard.h"

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

uint64_t score_groups(const const_bitboard &c,
        const vector<uint64_t> &gs,
        const Weights &ws,
        uint64_t other);
