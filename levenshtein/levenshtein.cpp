#include <iostream>
#include <string>
#include <vector>
using namespace std;


inline int min3(int a, int b, int c) {
    return min(min(a,b),c);
}

//C(i,j) = ...
//  ... i if i = M
//  ... j if j = N
//  ... C(i+1,j+1) if u_i = v_j
//  ... 1 + min(C(i+1,j), C(i,j+1), C(i,j))

struct action_t {
    string verb;
    int index;
    char c;
};

int ldistance(const string& u, const string& v) {
    typedef vector<int> row_t;
    typedef vector<row_t> matrix_t;
    matrix_t dp_matrix(u.length()+1, row_t(v.length()+1));
    for (unsigned i = 0; i <= u.length(); ++i)
        dp_matrix[i][v.length()] = u.length() - i;
    for (unsigned i = 0; i <= v.length(); ++i)
        dp_matrix[u.length()][i] = v.length() - i;
    for (int i = u.length() - 1; i >= 0; --i) {
        for (int j = v.length() - 1; j >= 0; --j) {
            if (u[i] == v[j])
                dp_matrix[i][j] = dp_matrix[i+1][j+1];
            else
                dp_matrix[i][j] = 1 + min3(dp_matrix[i+1][j],
                                           dp_matrix[i][j+1],
                                           dp_matrix[i+1][j+1]);
        }
    }
    
    return dp_matrix[0][0];
}

int main() {
    string u, v;
    cin >> u >> v;
    cout << ldistance(u, v) << endl;
}
