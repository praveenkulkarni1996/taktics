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

    int StandingCaptivesHard;
    int StandingCaptivesSoft;
    int CapstoneCaptivesHard;
    int CapstoneCaptivesSoft;
    int FlatCaptivesHard;
    int FlatCaptivesSoft;


    int Potential;
    int Threat;

    int EmptyControl;
    int FlatControl;

    int Center;
    int CenterControl;

    Weights() {
        assert(N == 5);
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

        GroupLiberties = 0;
        Liberties = 0;

        Potential = 100;
        Threat = 300;

        EmptyControl = 20;
        FlatControl = 50;

        Center = 40;
        CenterControl = 10;

        FlatCaptivesHard = 200;
        FlatCaptivesSoft = -200;

        StandingCaptivesHard = 300;
        StandingCaptivesSoft = -100;

        CapstoneCaptivesHard = 250;
        CapstoneCaptivesSoft = -100;
    }
};

const Weights WEIGHTS;
const const_bitboard CBOARD = precompute(N);

inline bitboard_t make_bitboard(const Board &board) {
    /* TODO: test this function */
    // const int N = 5;
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
            cout << (1 & (mask >> cnt--));
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
uint64_t score_groups(const vec64 &gs, uint64_t other) {
    uint64_t allg = 0;
    uint64_t sc = 0;
    for(const uint64_t &g : gs) {
        pair<int, int> wh = dimensions(CBOARD, g);
        int w = wh.first, h = wh.second;
        sc += WEIGHTS.Groups[w];
        sc += WEIGHTS.Groups[h];
        allg |= g;
    }
	return sc;
}

ipair64 count_one(const bitboard_t &p, const vec64 &gs, uint64_t pieces, uint64_t empty){
    int64_t place = 0, threat = 0;
    uint64_t singles = pieces;
    for(uint64_t g: gs) {
        singles &= ~g;
    }
    for(int i = 0; i < gs.size(); ++i) {
        uint64_t g = gs[i];
        if((g & CBOARD.edge) == 0) continue;
        uint64_t pmap = 0, tmap = 0;
        uint64_t slides = grow(CBOARD, CBOARD.mask&~(p.standing|p.caps), pieces & ~g);
        if ((g&CBOARD.L) != 0) {
            pmap |= (g >> 1) & empty & CBOARD.R;
            tmap |= (g >> 1) & slides & CBOARD.R;
        }
        if ((g&CBOARD.R) != 0) {
            pmap |= (g << 1) & empty & CBOARD.L;
            tmap |= (g << 1) & slides & CBOARD.L;
        }
        if ((g&CBOARD.T) != 0) {
            pmap |= (g >> CBOARD.size) & empty & CBOARD.B;
            tmap |= (g >> CBOARD.size) & slides & CBOARD.B;
        }
        if ((g&CBOARD.B) != 0) {
            pmap |= (g << CBOARD.size) & empty & CBOARD.T;
            tmap |= (g << CBOARD.size) & slides & CBOARD.T;
        }
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
            if (not (((g&CBOARD.L) != 0 && (other&CBOARD.R) != 0) ||
                ((g&CBOARD.R) != 0 && (other&CBOARD.L) != 0) ||
                ((g&CBOARD.B) != 0 && (other&CBOARD.T) != 0) ||
                ((g&CBOARD.T) != 0 && (other&CBOARD.B) != 0))) {
                continue;
            }
            uint64_t slides = grow(CBOARD, CBOARD.mask&~(p.standing|p.caps), pieces&~(g|other));
            uint64_t isect = grow(CBOARD, CBOARD.mask, g) & grow(CBOARD, CBOARD.mask, other);
            pmap |= isect & empty;
            tmap |= isect & slides;
        }
        place += popcount(pmap);
        threat += popcount(tmap);
    }
    return make_pair(place, threat);
}

iquad64 count_threats(const bitboard_t &p, vec64 &wgs, vec64 &bgs) {
    uint64_t empty = CBOARD.mask & ~(p.white|p.black);
    auto wp_wt = count_one(p, wgs, p.white&~(p.standing|p.caps), empty);
    auto bp_bt = count_one(p, bgs, p.black&~(p.standing|p.caps), empty);
    return make_pair(wp_wt, bp_bt);
}

int64_t score_threats(bitboard_t &p, bool player_color, vec64 wgs, vec64 bgs) {
	if (WEIGHTS.Potential == 0 && WEIGHTS.Threat == 0) {
		return 0;
	}
	iquad64 wp_wt_bp_bt = count_threats(p, wgs, bgs);
    int64_t wp = wp_wt_bp_bt.first.first;
    int64_t wt = wp_wt_bp_bt.first.second;
    int64_t bp = wp_wt_bp_bt.second.first;
    int64_t bt = wp_wt_bp_bt.second.second;

    // cout << "wp = " << wp << endl;
    // cout << "wt = " << wt << endl;
    // cout << "bp = " << bp << endl;
    // cout << "bt = " << bt << endl;

	if (wp+wt > 0 && player_color) {
		return 1 << 20;
	}
	if (bp+bt > 0 && not player_color) {
		return -(1 << 20);
	}
	return (int64_t)((wp-bp)*WEIGHTS.Potential) + (int64_t)((wt-bt)*WEIGHTS.Threat);
}
/**/
void compute_influence(uint64_t mine, vector<uint64_t> &out) {
    while(mine != 0) {
        uint64_t next = mine&(mine - 1);
        uint64_t bit = mine&~next;
        mine = next;
        uint64_t g = grow(CBOARD, CBOARD.mask, bit) & ~bit;
        uint64_t carry = g;
        for(int i = 0; carry != 0 and i < out.size(); ++i) {
            uint64_t cout = out[i] & carry;
            out[i] ^= carry;
            carry = cout;
        }
        if(carry != 0) {
            out[out.size() - 1] = carry;
        }
    }
}

upair64 compute_control(const bitboard_t &p) {
    vector<uint64_t> wi(3, 0), bi(3, 0);
    compute_influence(p.white &~(p.caps|p.standing), wi);
    uint64_t bc = 0, wc = 0;
    for(int i = wi.size() - 1; i >= 0; --i) {
        uint64_t wb = wi[i] & ~(wc|bc);
        uint64_t bb = bi[i] & ~(wc|bc);
        wc |= (wb & ~bb);
        bc |= (bb & ~wb);
    }
    uint64_t block = grow(CBOARD, CBOARD.mask, p.standing);
    uint64_t wcap = grow(CBOARD, CBOARD.mask, p.caps&p.white);
    uint64_t bcap = grow(CBOARD, CBOARD.mask, p.caps&p.black);
    wc |= wcap & ~bcap;
    bc |= bcap & ~wcap;
    wc &= ~block;
    bc &= ~block;
    return make_pair(wc, bc);
}

int64_t score_control(const bitboard_t &p) {
    if(WEIGHTS.EmptyControl == 0 and WEIGHTS.FlatControl == 0) return 0;
    upair64 wc_bc = compute_control(p);
    uint64_t wc = wc_bc.first;
    uint64_t bc = wc_bc.second;
	uint64_t empty = CBOARD.mask &~ (p.white | p.black);
	uint64_t flat = (p.white | p.black) &~ (p.standing | p.caps);
    int64_t s = 0;
	s += (int64_t)(WEIGHTS.EmptyControl * (popcount(wc&empty) - popcount(bc&empty)));
	s += (int64_t)(WEIGHTS.FlatControl * (popcount(wc&flat) - popcount(bc&flat)));
	s += (int64_t)(WEIGHTS.CenterControl * (popcount(wc&~CBOARD.edge) - popcount(bc&~CBOARD.edge)));
	return s;
}

/* The main evaluate function */
int64_t evaluate(const Board &board, bool player_color) {

    /* player color is the color of the player who has to make a move next */
    bitboard_t bb = make_bitboard(board);

    /* Handle terminal business */
    int64_t score = 0;
    int left = min(bb.white_stones, bb.black_stones);
    left = min(left, ENDGAME_CUTOFF);
    uint64_t flat = WEIGHTS.TopFlat
                    + ((ENDGAME_CUTOFF - left) * WEIGHTS.EndGameFlat / ENDGAME_CUTOFF);

    /* calculate the score based on the color */
    /* white has an advantage */
    if(player_color)    score += (flat / 2) + 50;
    else                score -= (flat / 2) + 50;
    // cerr << "player score = " << score << "\n";

    for(int i = 0; i < N; ++i) {
        for(int j = 0; j < N; ++j) {
            const int h = board.board[i][j].size();
            if(h <= 1) continue;
            int white_cnt = 0, black_cnt = 0;
            for(int climber = 0; climber < h - 1; ++climber) {
                switch (board.board[i][j][climber]) {
                    case WHITE_FLAT:
                    case WHITE_CRUSH: white_cnt++; break;
                    case BLACK_FLAT:
                    case BLACK_CRUSH: black_cnt++; break;
                    default: assert(false);
                }
            }
            const int sign = board.white(i, j) ? +1 : -1;
            const int sf = (sign == 1) ? white_cnt : black_cnt;
            const int hf = (sign == 1) ? black_cnt : white_cnt;

            switch(board.board[i][j].back()) {
                case WHITE_WALL:
                case BLACK_WALL:
                    score += sign * (hf * WEIGHTS.StandingCaptivesHard - sf * WEIGHTS.StandingCaptivesSoft);
                    break;
                case WHITE_CAP:
                case BLACK_CAP:
                    score += sign * (hf * WEIGHTS.CapstoneCaptivesHard - sf * WEIGHTS.CapstoneCaptivesHard);
                    break;
        		default:
                    score += sign * (hf * WEIGHTS.FlatCaptivesHard - sf  * WEIGHTS.FlatCaptivesHard);
                    break;
    		}
        }
    }

    /* scoring the naive board position */
    score += popcount(bb.white&(~(bb.caps|bb.standing))) * flat;
	score -= popcount(bb.black&(~(bb.caps|bb.standing))) * flat;
	score += popcount(bb.white&bb.standing) * WEIGHTS.Standing;
	score -= popcount(bb.black&bb.standing) * WEIGHTS.Standing;
	score += popcount(bb.white&bb.caps) * WEIGHTS.Capstone;
	score -= popcount(bb.black&bb.caps) * WEIGHTS.Capstone;
    // cerr << "naive score = " << score << "\n";

    /* scoring centre control */
    score += popcount(bb.white&~CBOARD.edge) * WEIGHTS.Center;
    score -= popcount(bb.black&~CBOARD.edge) * WEIGHTS.Center;
    // cerr << "centered score = " << score << "\n";

    /* scoring groups */
    vector<uint64_t> white_groups, black_groups;
    analyze(CBOARD, bb, white_groups, black_groups);

    score += score_groups(white_groups, bb.black|bb.standing);
	score -= score_groups(black_groups, bb.white|bb.standing);
    // cerr << "group score = " << score << "\n";

    // if (WEIGHTS.Liberties != 0) {
    //     uint64_t wr = bb.white&~bb.standing;
    //     uint64_t br = bb.black&~bb.standing;
    //     uint64_t wl = popcount(grow(CBOARD, ~bb.black, wr) & (~bb.white));
    //     uint64_t bl = popcount(grow(CBOARD, ~bb.white, br) & (~bb.black));
    //     score += WEIGHTS.Liberties * wl;
    //     score -= WEIGHTS.Liberties * bl;
    // }

    score += score_threats(bb, player_color, white_groups, black_groups);
    // cout << "score threats = " << score << "\n";
    score += score_control(bb);
    // cout << "score control = " << score << "\n";

    // cout << "final abs score = " << score << "\n";
    // cout << "final neg score = " << -score << "\n";
    return score;
    // return (player_color? score: -score);
}
