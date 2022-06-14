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

void getgraph(const string &filename, unordered_map<int, unordered_set<int>> &g)
{
    g.clear();
    ifstream fin(filename);
    int a, b;
    while (fin >> a >> b)
    {
        g[a].insert(b);
    }
}
struct myCmp
{
    bool operator()(const pair<int, int> &a, const pair<int, int> &b)
    {
        if (a.first == b.first)
            return false;
        else
        {
            if (a.second != b.second)
                return a.second < b.second;
            else
                return a.first < b.first;
        }
    }
};

void coreDecomp(unordered_map<int, unordered_set<int>> g, unordered_map<int, int> &core)
{
    set<pair<int, int>, myCmp> s;
    unordered_map<int, int> deg;
    for (auto &it : g)
    {
        deg[it.first] = int(it.second.size());
        s.insert(make_pair(it.first, deg[it.first]));
    }
    int K = 0;
    while (!s.empty())
    {
        pair<int, int> p = *s.begin();
        s.erase(s.begin());
        K = max(K, p.second);
        core[p.first] = K;
        for (auto node : g[p.first])
        {
            g[node].erase(p.first);
            if (s.erase(make_pair(node, deg[node])))
            {
                deg[node] = int(g[node].size());
                s.insert(make_pair(node, deg[node]));
            }
        }
        deg.erase(p.first);
        g.erase(p.first);
    }
}
void solve(const string &str)
{
    string filepath = "./result/NormalGraph/";
    string filename = filepath + str;
    unordered_map<int, unordered_set<int>> g;
    getgraph(filename, g);
    unordered_map<int, int> core;
    coreDecomp(g, core);
    ofstream fout("./result/NormalCoreNumber/" + str + "-coreValue");
    for (auto &it : core)
    {
        fout << it.first << ' ' << it.second << endl;
    }
    fout << endl;
    cout << "pid=" << std::this_thread::get_id() << ' ' << str << " finish!" << endl;
}

int main()
{
    //S_CoGe  S_CoHi  S_DAWN  S_DrAb  S_MaAn  S_MiAc  S_NDC  S_StAn  S_TrCl  S_WaTr T_CoBi T_CoDB T_ThAU T_ThSO
    string file = "S_CoGe  S_CoHi  S_DAWN  S_DrAb  S_MiAc  S_NDC  S_TrCl  S_WaTr ";
    istringstream ss(file);
    string str;
    // std::vector<std::thread> thread_vec;
    ofstream fout("./result/normalgraph-statistic.txt");
    while (ss >> str)
    {
        // thread_vec.emplace_back(std::thread(solve, str));
        string filepath = "./result/NormalGraph/";
        string filename = filepath + str;
        unordered_map<int, unordered_set<int>> g;
        getgraph(filename, g);
        // unordered_map<int, int> core;
        // auto t5 = std::chrono::steady_clock::now();
        // coreDecomp(g, core);
        // auto t6 = std::chrono::steady_clock::now();
        // auto dr_ns3 = std::chrono::duration<double, std::nano>(t6 - t5).count();
        // fout<<str<<' '<<dr_ns3<<endl;
        int v = int(g.size());
        int edge=0;
        int count=0;
        int maxdeg=INT_MIN;
        for(auto& it:g)
        {
            edge+=int(it.second.size());
            count+=int(it.second.size());
            maxdeg=max(maxdeg,int(it.second.size()));
        }
        edge/=2;
        fout<<str<<' '<<v<<' '<<edge<<' '<<maxdeg<<' '<<count*1.0/v<<endl;
    }
    fout.close();
    // for (auto &it : thread_vec)
    // {
    //     it.join();
    // }
    cout << "全部完成" << endl;
}