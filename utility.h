#include <string> 
using namespace std;

bool out_of_bounds(const int x, const int y, const int N=5);
int next_x(int x, char dir);
int next_y(int y, char dir);
string make_sqr(int x, int y, int N=5);