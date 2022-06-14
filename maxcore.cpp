#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <set>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <chrono>
#include <map>
#include <climits>
#include <algorithm>
#include <random>
#include <thread>
using namespace std;

int main()
{
    string file = "S_CoGe  S_CoHi  S_DAWN  S_DrAb  S_MaAn  S_MiAc  S_NDC  S_TrCl  S_WaTr T_CoBi T_CoDB T_ThAU T_ThSO";
    istringstream ss(file);
    string str;
    ofstream fout("./result/maxCore.txt");
    fout << "name hypergraph normalgraph" << endl;
    while (ss >> str)
    {
        ifstream fin("./result/CoreNumber/" + str + "-coreVertexValue");
        int a, b;
        int x = INT_MIN;
        while (fin >> a >> b)
        {
            x = max(x, b);
        }
        fin.close();

        fin.open("./result/NormalCoreNumber/" + str + "-coreValue");
        int y = INT_MIN;
        while (fin >> a >> b)
        {
            y = max(y, b);
        }
        fin.close();
        fout << str << ' ' << x << ' ' << y << endl;
        cout << str << endl;
    }
    fout.close();
}