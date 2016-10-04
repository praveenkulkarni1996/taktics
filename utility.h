#include <string>
#include <vector>
using namespace std;

enum Stones {
    BLACK_FLAT,
    BLACK_WALL,
    BLACK_CAP,
    BLACK_CRUSH,
    WHITE_FLAT,
    WHITE_WALL,
    WHITE_CAP,
    WHITE_CRUSH
};

typedef const string Move;
typedef vector<Move> Moves;

bool out_of_bounds(const int x, const int y, const int N=5);
int next_x(const int x, const char dir);
int next_y(const int y, const char dir);
string make_sqr(const int x, const int y, const int N=5);
void print_moves(Moves moves);
pair<int, int> make_xy(const char x, const char y);

bool check_white(const Stones &stone);
bool check_black(const Stones &stone);
