#include "board.h"
#include <cassert>
#include <iostream>
using namespace std;

int Board::evaluate_captives(const bool player_color) const {
    const int FLAT_CAPTIVES[] = {-200, 200};
    const int WALL_CAPTIVES[] = {-150, 300};
    const int CAPS_CAPTIVES[] = {-150, 250};
    int score = 0;
    for(int x = 0; x < N; ++x) {
        for(int y = 0; y < N; ++y) {
            if(height(x, y) <= 1) continue;
            bool top_color = white(x, y);
            for(int h = 0; h < (int)(board[x][y].size() - 1); ++h) {
                bool stone_color = check_white(board[x][y][h]);
                if(stone_color != player_color) continue;
                const int same = (top_color == stone_color);
                if(caps(x, y)) score += CAPS_CAPTIVES[same];
                else if(wall(x, y)) score += WALL_CAPTIVES[same];
                else if(flat(x, y)) score += FLAT_CAPTIVES[same];
                else if(crush(x, y)) score += FLAT_CAPTIVES[same];
                else assert(false);
            }
        }
    }
    return score;
}

int Board::evaluate_tops(const bool player_color) const {
    const int FLAT = 400;
    const int WALL = 200;
    const int CAPS = 300;
    int score = 0;
    for(int x = 0; x < N; ++x) {
        for(int y = 0; y < N; ++y) {
            if(white(x, y) == player_color) {
                if(white_cap(x, y) || black_cap(x, y)) {
                    score += CAPS;
                }
                else if(white_flat(x, y) || black_flat(x, y)) {
                    score += FLAT;
                }
                else if(white_wall(x, y) || black_wall(x, y)) {
                    score += WALL;
                }
                else if(white_crush(x, y) || black_crush(x, y)) {
                    score += FLAT;
                }
                else assert(empty(x, y));
            }
        }
    }
    return score;
}

bool Board::perform_placement(Move move, bool white) {
    const pair<int, int> xy = make_xy(move[1], move[2]);
    const int x = xy.first;
    const int y = xy.second;
    // cout << "x = " << x << " y = " << y << "\n";
    if(white) {
        switch(move[0]) {
            case 'F': board[x][y].push_back(WHITE_FLAT); white_flats_rem--; break;
            case 'S': board[x][y].push_back(WHITE_WALL); white_flats_rem--; break;
            case 'C': board[x][y].push_back(WHITE_CAP);  white_caps_rem--; break;
            default : assert(false);
        }
    } else {
        switch(move[0]) {
            case 'F': board[x][y].push_back(BLACK_FLAT); black_flats_rem--; break;
            case 'S': board[x][y].push_back(BLACK_WALL); black_flats_rem--; break;
            case 'C': board[x][y].push_back(BLACK_CAP);  black_caps_rem--; break;
            default : assert(false);
        }
    }
    return false; // because you cannot crush in a placement
}

bool Board::perform_motion(Move move, bool white) {
    // Assumes a valid move
    /* returns if you crushed or not */
    int h = move[0] - '0';
    bool crush = false;
    const pair<int, int> xy = make_xy(move[1], move[2]);
    int x = xy.first;
    int y = xy.second;

    assert(not board[x][y].empty());
    assert(this->white(x, y) == white);

    const char dir = move[3];
    // cout << "x = " << x << " y = " << y << "\n";
    vector<Stones> pickup;
    vector<int> drops;
    assert((int)board[x][y].size() >= h);
    for(int i = 4; i < (int)move.length(); ++i)
        drops.push_back(move[i] - '0');
    for(int i = 0; i < h; ++i) {
        Stones stone = board[x][y].back();
        board[x][y].pop_back();
        pickup.push_back(stone);
    }
    for(int i = 0; i < (int)drops.size(); ++i) {
        x = next_x(x, dir);
        y = next_y(y, dir);
        assert(not out_of_bounds(x, y));
        for(int j = 0; j < drops[i]; ++j) {
            Stones stone = pickup.back();
            pickup.pop_back();
            if(board[x][y].empty() == false) {
                if(board[x][y].back() == WHITE_WALL or board[x][y].back() == BLACK_WALL) {
                    crush = true;
                    // cerr << "crushed at " << x << ", " << y << "\n";
                    assert(stone == WHITE_CAP or stone == BLACK_CAP);
                    board[x][y].back() = (board[x][y].back() == WHITE_WALL)
                                        ? WHITE_CRUSH : BLACK_CRUSH;
                }
            }
            board[x][y].push_back(stone);
        }
    }
    return crush;
}

void Board::undo_placement(const Move move, const bool player_color) {
    const pair<int, int> xy = make_xy(move[1], move[2]);
    const int x = xy.first;
    const int y = xy.second;
    assert(board[x][y].size() == 1);
    assert(this->white(x, y) == player_color);
    switch (board[x][y].back()) {
      case WHITE_FLAT :
      case WHITE_WALL : ++white_flats_rem; break;
      case WHITE_CAP : ++white_caps_rem; break;
      case BLACK_FLAT :
      case BLACK_WALL : ++black_flats_rem; break;
      case BLACK_CAP : ++black_caps_rem; break;
      default: assert(false);
    }
    board[x][y].pop_back();
}

void Board::undo_motion(Move move, bool white, bool uncrush) {
    /*  read the height of the stack */
    const char dir = (move[3]);
    /* read the final coordinates */
    const pair<int, int> xy = make_xy(move[1], move[2]);
    const int x0 = xy.first;
    const int y0 = xy.second;
    int x = xy.first;
    int y = xy.second;
    vector<Stones> temp;
    vector<int> drops;
    for(int i = 4; i < (int)move.length(); ++i) {
        drops.push_back(move[i] - '0');
    }
    for(int i = 0; i < (int)drops.size(); ++i) {
        x = next_x(x, dir), y = next_y(y, dir);
        for(int j = 0; j < drops[i]; ++j) {
            Stones stone = board[x][y].back();
            board[x][y].pop_back();
            temp.push_back(stone);
        }
        assert(drops[i] == (int)temp.size());
        for(int j = 0; j < drops[i]; ++j) {
            Stones stone = temp.back();
            temp.pop_back();
            board[x0][y0].push_back(stone);
        }
    }
    if(uncrush == true) {
        assert(board[x][y].back() == BLACK_CRUSH or board[x][y].back() == WHITE_CRUSH);
        assert(board[x0][y0].back() == BLACK_CAP or board[x0][y0].back() == WHITE_CAP);
        // cerr << "uncrushed at x = " << x << ", y = " << y << "\n";
        board[x][y].back() = (board[x][y].back() == BLACK_CRUSH) ? BLACK_WALL : WHITE_WALL;
    }
    assert(this->white(x0, y0) == white);
}

int Board::evaluate(bool player_color) {
    return evaluate_helper(player_color) - evaluate_helper(not player_color);
}

int Board::evaluate_helper(bool player_color) {
    return evaluate_captives(player_color) + evaluate_tops(player_color);
}

bool Board::perform_move(const Move &move, bool white) {
    if(move[0] == 'F' || move[0] == 'S' || move[0] == 'C')
        return perform_placement(move, white);
    else
        return perform_motion(move, white);
}
void Board::undo_move(const Move &move, bool white, bool uncrush) {
    if(move[0] == 'F' || move[0] == 'S' || move[0] == 'C') {
        assert(uncrush == false);
        return undo_placement(move, white);
    }
    else return undo_motion(move, white, uncrush);
}

bool Board::player_road_win(const bool player_color) const {
    bool reach[N][N];
    memset(reach, false, sizeof(reach));
    for(int x = 0; x < N; ++x) {
        if(x == 0) for(int y = 0; y < N; ++y) {
            reach[x][y] = (road_piece(x, y) and (white(x, y) == player_color));
        }
        else {
            assert(x >= 1 && x < N);
            for(int y = 0; y < N; ++y) {
                reach[x][y] |= reach[x-1][y] and (road_piece(x, y) and (white(x, y) == player_color));
            }
            for(int y = 1; y < N; ++y) {
                reach[x][y] |= reach[x][y-1] and (road_piece(x, y) and (white(x, y) == player_color));
            }
            for(int y = N-2; y >= 0; --y) {
                reach[x][y] |= reach[x][y+1] and (road_piece(x, y) and (white(x, y) == player_color));
            }
        }
    }
    bool game_over = false;
    for(int y = 0; y < N; ++y) game_over = game_over or reach[N-1][y];
    if(game_over) return true;
    memset(reach, false, sizeof(reach));
    for(int y = 0; y < N; ++y) {
        if(y == 0) for(int x = 0; x < N; ++x) {
            reach[x][y] = (road_piece(x, y) and (white(x, y) == player_color));
        }
        else {
            assert(y >= 1 and y < N);
            for(int x = 0; x < N; ++x) {
                reach[x][y] |= reach[x][y-1] and (road_piece(x, y) and (white(x, y) == player_color));
            }
            for(int x = 1; x < N; ++x) {
                reach[x][y] |= reach[x-1][y] and (road_piece(x, y) and (white(x, y) == player_color));
            }
            for(int x = N-2; x >= 0; --x) {
                reach[x][y] |= reach[x+1][y] and (road_piece(x, y) and (white(x, y) == player_color));
            }
        }
    }
    for(int x = 0; x < N; ++x) game_over = game_over or reach[x][N-1];
    return game_over;
}

bool Board::game_flat_win() const {
    bool space_found = false;
    for(int x = 0; x < N; ++x) {
        for(int y = 0; y < N; ++y) {
            if(empty(x, y)) return false;
        }
    }
    return true;
}

bool Board::player_flat_win(const bool player_color) const {
    // assumes that flat win holds
    // assumes that top of stack color means that you own the piece
    // returns true if the player draws or wins
    int black_squares = 0, white_squares = 0;
    for(int x = 0; x < N; ++x) {
        for(int y = 0; y < N; ++y) {
            if(white(x, y)) white_squares++;
            else if(black(x, y)) black_squares++;
            else assert(empty(x, y));
        }
    }
    if(black_squares != white_squares) 
        return ((black_squares < white_squares) == player_color);
    else if(black_flats_rem != white_flats_rem) 
        return ((black_flats_rem < white_flats_rem) == player_color);
    else
        return true;
}