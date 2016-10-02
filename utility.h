#include <string> 
#include <vector>
using namespace std;

typedef string Move;
typedef vector<Move> Moves;

bool out_of_bounds(const int x, const int y, const int N=5);
int next_x(int x, char dir);
int next_y(int y, char dir);
string make_sqr(int x, int y, int N=5);
void print_moves(Moves moves);
pair<int, int> make_xy(char x, char y);