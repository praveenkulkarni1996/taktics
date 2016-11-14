#include <cstdint>
#include "board.h"

struct const_bitboard {
    int size;
    uint64_t L, R, T, B;
    uint64_t edge;
    uint64_t mask;
};

struct bitboard_t {
    uint64_t standing;
    uint64_t caps;
    uint64_t white;
    uint64_t black;
    uint64_t white_stones;
    uint64_t black_stones;

    bitboard_t() {
        standing = caps = white = black = 0;
        white_stones = black_stones = 0;
    }
};

const_bitboard precompute(const int size) {
    const_bitboard c;
    c.size = size;
    for(int i = 0; i < size; ++i) {
        c.R |= 1LL << (i * size);
    }
    c.L = c.R << (size - 1LL);
    c.T = ((1LL << size) - 1LL) << (size * (size - 1LL));
    c.B = (1LL << size) - 1;
    c.mask = (1<<(size*size)) - 1;
    c.edge = c.L | c.R | c.B | c.T;
    return c;
}

inline int popcount(uint64_t x) {
    int count = 0;
    while(x > 0) {
        count += (x&1);
        x >>= 1;
    }
    return count;
    /*
    if (x == 0) return 0;
    x -= (x >> 1) & 0x5555555555555555;
    x = (x>>2)&0x3333333333333333 + x&0x3333333333333333;
    x += x >> 4;
    x &= 0x0f0f0f0f0f0f0f0f;
    x *= 0x0101010101010101;
    return int(x >> 56);
    */
}

inline uint64_t grow(const const_bitboard &c, const uint64_t within, const uint64_t seed) {
    uint64_t next = seed;
    next |= (seed << 1) & (~c.R);
    next |= (seed >> 1) & (~c.L);
    next |= (seed >> c.size);
    next |= (seed << c.size);
    return next & within;
}

uint64_t flood(const const_bitboard &c, uint64_t within, uint64_t seed) {
    while(true) {
        uint64_t next = grow(c, within, seed);
        if(next == seed) {
            return next;
        }
        seed = next;
    }
}

/* this updates out */
void flood_groups(const const_bitboard &c, uint64_t bits, vector<uint64_t> &out) {
    uint64_t seen = 0;
    while(bits != 0) {
        uint64_t next = bits & (bits - 1);
        uint64_t bit = bits & (~next);
        if((seen & bit) == 0) {
            uint64_t g = flood(c, bits, bit);
            if(g != bit) {
                out.push_back(g);
            }
            seen |= g;
        }
        bits = next;
    }
}

void analyze(const const_bitboard &c,
        const bitboard_t &bb,
        vector<uint64_t> &white_groups,
        vector<uint64_t> &black_groups) {
    assert(white_groups.empty());
    assert(black_groups.empty());
    uint64_t white_roads = bb.white & ~bb.standing;
    uint64_t black_roads = bb.black & ~bb.standing;
    flood_groups(c, white_roads, white_groups);
    flood_groups(c, black_roads, black_groups);
}

pair<int, int> dimensions(const const_bitboard &c, uint64_t bits) {
    int w = 0, h = 0;
    if(bits == 0) {
        return make_pair(0, 0);
    }
    uint64_t b = c.L;
    while((bits & b) == 0) {
        b >>= 1;
    }
    while(b != 0 and (bits & b) != 0) {
        b >>= 1;
        w++;
    }
    while(b != 0 and ((bits & b) != 0)) {
        b >>= 1;
        w++;
    }
    b = c.T;
    while((bits&b) == 0) {
        b >>= c.size;
    }
    while (b != 0 and (bits&b) != 0) {
        b >>= c.size;
        h++;
    }
    return make_pair(w, h);
}
