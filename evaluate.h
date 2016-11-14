#include <cstdint>
#include "bitboard.h"

typedef pair<int64_t, int64_t> ipair64; /* signed int64_t pair */
typedef pair<uint64_t, uint64_t> upair64; /* unsigned uint64_t pair */
typedef pair<ipair64, ipair64> iquad64; /* signed quadruple int64_t */
typedef vector<uint64_t> vec64;
typedef const const_bitboard cboard;

const int ENDGAME_CUTOFF = 7;

struct Weights {
    int TopFlat;
    int EndGameFlat;
    int Standing;
    int Capstone;

    int Groups[8];
    int GroupLiberties;
    int Liberties;

    int Potential;
    int Threat;

    int EmptyControl;
    int FlatControl;

    int Center;
    int CenterControl;

    Weights() {
        TopFlat = 400;
        EndGameFlat = 800;
        Standing = 200;
    	Capstone = 300;

        Groups[0] = 0;
        Groups[1] = 0;
        Groups[2] = 0;
        Groups[3] = 100;
        Groups[4] = 300;
        Groups[5] = 500;
        Groups[6] = 0;
        Groups[7] = 0;

        Potential = 100;
        Threat = 300;

        EmptyControl = 20;
        FlatControl = 50;

        Center = 40;
        CenterControl = 10;
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
    bitboard.white_stones = 20; //board.white_flats_rem;
    bitboard.black_stones = 20; //board.black_flats_rem;
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

/* scores the groups */
uint64_t score_groups(const const_bitboard &c, const vector<uint64_t> &gs, const Weights &ws, uint64_t other) {
    uint64_t allg = 0;
    uint64_t sc = 0;
    for(const uint64_t &g : gs) {
        pair<int, int> wh = dimensions(c, g);
        int w = wh.first, h = wh.second;
        sc += ws.Groups[w];
        sc += ws.Groups[h];
        allg |= g;
    }
    cerr << "sc = " << sc << "\n";
	if (ws.GroupLiberties != 0) {
		int libs = popcount(grow(c, ~other, allg) & (~allg));
		sc += libs * ws.GroupLiberties;
	}
	return sc;
}

ipair64 count_one(cboard &c, const bitboard_t &p, const vec64 &gs, uint64_t pieces, uint64_t empty){
    int64_t place = 0, threat = 0;
    uint64_t singles = pieces;
    for(uint64_t g: gs) {
        singles &= ~g;
    }
    cout << "singles " << "\n";
    print_bitmask(singles);
    for(int i = 0; i < gs.size(); ++i) {
        uint64_t g = gs[i];
        if((g & c.edge) == 0) continue;
        uint64_t pmap = 0, tmap = 0;
        uint64_t slides = grow(c, c.mask&~(p.standing|p.caps), pieces & ~g);
        cout << "slides = \n"; print_bitmask(slides);
        cout << "g = \n"; print_bitmask(g);
        cout << "empty = \n"; print_bitmask(empty);
        if ((g&c.L) != 0) {
            pmap |= (g >> 1) & empty & c.R;
            tmap |= (g >> 1) & slides & c.R;
        }
        if ((g&c.R) != 0) {
            pmap |= (g << 1) & empty & c.L;
            tmap |= (g << 1) & slides & c.L;
        }
        if ((g&c.T) != 0) {
            pmap |= (g >> c.size) & empty & c.B;
            tmap |= (g >> c.size) & slides & c.B;
        }
        if ((g&c.B) != 0) {
            pmap |= (g << c.size) & empty & c.T;
            tmap |= (g << c.size) & slides & c.T;
        }
        cout << "pmap\n"; print_bitmask(pmap);
        uint64_t s = singles;
        uint64_t j = 0;
        while(true) {
            uint64_t other = 0;
            if(j < i) {
                other = gs[j];
                j++;
            } else if (s != 0) {
                uint64_t next = s & (s - 1);
                other = s & ~next;
                s = next;
            } else {
                break;
            }
            if (not (((g&c.L) != 0 && (other&c.R) != 0) ||
                ((g&c.R) != 0 && (other&c.L) != 0) ||
                ((g&c.B) != 0 && (other&c.T) != 0) ||
                ((g&c.T) != 0 && (other&c.B) != 0))) {
                continue;
            }
            uint64_t slides = grow(c, c.mask&~(p.standing|p.caps), pieces&~(g|other));
            cout << "slides 2 " << "\n"; print_bitmask(slides);
            uint64_t isect = grow(c, c.mask, g) & grow(c, c.mask, other);
            pmap |= isect & empty;
            tmap |= isect & slides;
        }
        printf("pmap\n\n\n\n"); print_bitmask(pmap);
        cout << popcount(pmap);
        place += popcount(pmap);
        threat += popcount(tmap);
    }
    cout << "place = " << place << "\n";
    return make_pair(place, threat);
}

iquad64 count_threats(const const_bitboard &c, const bitboard_t &p, vec64 &wgs, vec64 &bgs) {
    uint64_t empty = c.mask & ~(p.white|p.black);
    auto wp_wt = count_one(c, p, wgs, p.white&~(p.standing|p.caps), empty);
    auto bp_bt = count_one(c, p, bgs, p.black&~(p.standing|p.caps), empty);
    return make_pair(wp_wt, bp_bt);
}

int64_t score_threats(cboard &c, const Weights &ws, bitboard_t &p, bool player_color, vec64 wgs, vec64 bgs) {
	if (ws.Potential == 0 && ws.Threat == 0) {
		return 0;
	}
	iquad64 wp_wt_bp_bt = count_threats(c, p, wgs, bgs);
    int64_t wp = wp_wt_bp_bt.first.first;
    int64_t wt = wp_wt_bp_bt.first.second;
    int64_t bp = wp_wt_bp_bt.second.first;
    int64_t bt = wp_wt_bp_bt.second.second;

    cout << "wp = " << wp << endl;
    cout << "wt = " << wt << endl;
    cout << "bp = " << bp << endl;
    cout << "bt = " << bt << endl;

	if (wp+wt > 0 && player_color) {
		return 1 << 20;
	}
	if (bp+bt > 0 && not player_color) {
		return -(1 << 20);
	}
	return (int64_t)((wp-bp)*ws.Potential) + (int64_t)((wt-bt)*ws.Threat);
}


/* The main evaluate function */
int64_t evaluate(const Board &board, const Weights &w, const const_bitboard &c, bool player_color) {
    /* player color is the color of the player who has to make a move next */
    bitboard_t bb = make_bitboard(board);

    /* Handle terminal business */
    int64_t score = 0;
    int left = min(bb.white_stones, bb.black_stones);
    left = min(left, ENDGAME_CUTOFF);
    cerr << "left = " << left << "\n";
    uint64_t flat = w.TopFlat + ((ENDGAME_CUTOFF - left) * w.EndGameFlat / ENDGAME_CUTOFF);

    cerr << "flat = " << flat << "\n";

    /* calculate the score based on the color */
    /* white has an advantage */
    if(player_color)    score += (flat / 2) + 50;
    else                score -= (flat / 2) + 50;
    cerr << "player score = " << score << "\n";

    /* scoring the naive board position */
    score += popcount(bb.white&(~(bb.caps|bb.standing))) * flat;
	score -= popcount(bb.black&(~(bb.caps|bb.standing))) * flat;
	score += popcount(bb.white&bb.standing) * w.Standing;
	score -= popcount(bb.black&bb.standing) * w.Standing;
	score += popcount(bb.white&bb.caps) * w.Capstone;
	score -= popcount(bb.black&bb.caps) * w.Capstone;
    cerr << "naive score = " << score << "\n";

    /* scoring centre control */
    score += popcount(bb.white&~c.edge) * w.Center;
    score -= popcount(bb.black&~c.edge) * w.Center;
    cerr << "centered score = " << score << "\n";

    /* scoring groups */
    vector<uint64_t> white_groups, black_groups;
    analyze(c, bb, white_groups, black_groups);
    score += score_groups(c, white_groups, w, bb.black|bb.standing);
	score -= score_groups(c, black_groups, w, bb.white|bb.standing);
    cerr << "group score = " << score << "\n";

    cout << "liberties = " << w.Liberties << "\n";

    if (w.Liberties != 0) {
        uint64_t wr = bb.white&~bb.standing;
        uint64_t br = bb.black&~bb.standing;
        uint64_t wl = popcount(grow(c, ~bb.black, wr) & (~bb.white));
        uint64_t bl = popcount(grow(c, ~bb.white, br) & (~bb.black));
        score += w.Liberties * wl;
        score -= w.Liberties * bl;
    }

    /* BUG HERE */
    score += score_threats(c, w, bb, player_color, white_groups, black_groups);
    cout << "Score = " << score << "\n";
    // score += score_control(c, w, bb);

    return (player_color) ? score : -score;
}
