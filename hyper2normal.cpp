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

typedef pair<int, int> PII;

void getGraph(const string &filename, vector<vector<int>> &hyperEdge, unordered_map<int, vector<int>> &hyperNode)
{
    ifstream fin(filename, ios::in);
    int count = -1;
    while (true)
    {
        string str;
        getline(fin, str);
        if (str == "")
            break;
        istringstream ss(str);
        int tmp;
        vector<int> e;
        while (ss >> tmp)
        {
            if (find(e.begin(), e.end(), tmp) == e.end())
                e.push_back(tmp);
        }
        if (e.size() == 1)
            continue;
        count++;
        hyperEdge.push_back(e);
        for (auto &node : e)
            hyperNode[node].push_back(count);
    }
}

struct myCmp
{
    bool operator()(const pair<int, int> &a, const pair<int, int> &b) const
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

void solve(const string &str)
{
    string filepath = "/home/liuyu/C++Projects/graphData/";
    string filename = filepath + str;
    vector<vector<int>> hyperEdge;
    unordered_map<int, vector<int>> hyperNode;
    getGraph(filename, hyperEdge, hyperNode);
    unordered_map<int, unordered_set<int>> node2node; //普通子图
    for (auto &it : hyperNode)
    {
        for (auto &edge : it.second)
        {
            for (auto &node : hyperEdge[edge])
            {
                node2node[it.first].insert(node);
            }
        }
        node2node[it.first].erase(it.first);
    }

    ofstream fout("./result/NormalGraph/" + str);
    for (auto &it : node2node)
    {
        for (auto &node : it.second)
        {
            fout << it.first << ' ' << node << endl;
        }
    }
    fout.close();
   cout << "pid=" << std::this_thread::get_id() << ' ' << str << " finish!" << endl; 
}

int main()
{
    //S_CoGe  S_CoHi  S_DAWN  S_DrAb  S_MaAn  S_MiAc  S_NDC  S_StAn  S_TrCl  S_WaTr T_CoBi T_CoDB T_ThAU T_ThSO
    string file = "S_StAn";
    string filepath = "/home/liuyu/C++Projects/graphData/";
    istringstream ss(file);
    string str;
    std::vector<std::thread> thread_vec;
    while (ss >> str)
    {
        thread_vec.emplace_back(std::thread(solve, str));
    }
    for (auto &it : thread_vec)
    {
        it.join();
    }
    cout << "全部完成" << endl;
}