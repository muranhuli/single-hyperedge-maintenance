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
using namespace std;

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

/**
 * @brief Get the Graph object 读取一个超图的超边集和超点集
 *
 * @param filename 读取图的文件
 * @param hyperEdge 超边集，存储每一条边对应的点
 * @param hyperNode 超点集，存储每一个点对应的边
 */
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

void dividDataSet(vector<vector<int>> &hyperEdge, vector<vector<int>> &InsertEdge, int n)
{
    for (int i = 0; i < n; i++)
    {
        InsertEdge.emplace_back(hyperEdge[i]);
    }
    // for (int i = 0; i < int(InsertEdge.size()); i++)
    // {
    hyperEdge.erase(hyperEdge.begin(), hyperEdge.begin() + n);
    // }
}

void initial(vector<vector<int>> &hyperEdge, unordered_map<int, vector<int>> &hyperNode)
{
    hyperNode.clear();
    for (int i = 0; i < int(hyperEdge.size()); i++)
    {
        for (auto node : hyperEdge[i])
            hyperNode[node].emplace_back(i);
    }
}

void kCoreDecomp(vector<vector<int>> &hyperEdge, unordered_map<int, vector<int>> &hyperNode, unordered_map<int, int> &core)
{
    core.clear();
    set<pair<int, int>, myCmp> node_count;
    unordered_map<int, int> deg;
    vector<bool> visitEdge(hyperEdge.size(), false);
    unordered_map<int, bool> visitNode;
    for (auto &it : hyperNode)
    {
        deg[it.first] = it.second.size();
        node_count.insert(make_pair(it.first, deg[it.first]));
        visitNode[it.first] = false;
    }
    int K = 0;
    while (!node_count.empty())
    {
        pair<int, int> p = *node_count.begin();
        node_count.erase(node_count.begin());
        K = max(K, p.second);
        core[p.first] = K;
        visitNode[p.first] = true;
        for (auto &edge : hyperNode[p.first])
        {
            if (visitEdge[edge])
                continue;
            visitEdge[edge] = true;
            for (auto &node : hyperEdge[edge])
            {
                if (node == p.first)
                    continue;
                node_count.erase(make_pair(node, deg[node]));
                deg[node]--;
                node_count.insert(make_pair(node, deg[node]));
            }
        }
    }
}
int main()
{
    string file = "CoDB TaSO ThSO";
    string filepath = "/home/liuyu/C++Projects/graphData/hypergraph/";
    istringstream ss(file);
    string str;
    while (ss >> str)
    {
        ofstream fout1("./result/coreDecompTime/" + str + ".txt");
        string filename = filepath + str;
        int num = 1000;
        for (int i = 0; i <= num; i = i + 500)
        {
            cout << str << ' ' << i << endl;
            vector<vector<int>> hyperEdge;
            unordered_map<int, vector<int>> hyperNode;
            unordered_map<int, int> core;
            getGraph(filename, hyperEdge, hyperNode);
            cout << "*" << endl;
            vector<vector<int>> InsertEdge;
            dividDataSet(hyperEdge, InsertEdge, i);
            initial(hyperEdge, hyperNode);
            for (int k = 0; k < 5; k++)
            {
                auto t1 = std::chrono::steady_clock::now();
                kCoreDecomp(hyperEdge, hyperNode, core);
                auto t2 = std::chrono::steady_clock::now();
                double dr_ns2 = std::chrono::duration<double, std::nano>(t2 - t1).count();
                fout1 << dr_ns2 << ' ';
            }
            fout1<<endl;
            cout << "****" << endl;
        }
        fout1.close();
        // ofstream fout("./result/CoreNumber/"+str+"-coreVertexValue");
        // for (auto &it : core)
        // {
        //     fout << it.first<<' '<<it.second<<endl;
        // }
        // fout.close();

        // vector<int> coreE(int(hyperEdge.size()), 0);
        // for (int i = 0; i < int(hyperEdge.size()); i++)
        // {
        //     int mink = INT_MAX;
        //     for (auto node : hyperEdge[i])
        //         mink = min(mink, core[node]);
        //     coreE[i] = mink;
        // }
        // fout.open("./result/CoreNumber/"+str+"-coreEdgeValue");
        // for (auto &it : coreE)
        // {
        //     fout << it << endl;
        // }
        // fout.close();
        // cout << str << endl;
    }
    // fout1.close();
}