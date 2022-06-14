// DynamicInsertionOfHypergraph.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
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

void findHypergraphSubCore1(const vector<vector<int>> &hyperEdge, const unordered_map<int, vector<int>> &hyperNode, const unordered_map<int, int> &core, int u,
                            unordered_map<int, vector<int>> &preEdge, unordered_map<int, int> &cd)
{
    unordered_map<int, bool> visitNode;
    queue<int> Q;
    int k = core.at(u);
    Q.push(u);
    visitNode[u] = true;
    vector<int> visitEdge(hyperEdge.size(), false);
    while (!Q.empty())
    {
        int v = Q.front();
        Q.pop();

        for (auto &edge : hyperNode.at(v))
        {
            int minK = INT_MAX;
            for (auto &w : hyperEdge[edge])
                minK = min(minK, core.at(w));
            if (minK == k)
            {
                cd[v]++;
                preEdge[v].push_back(edge);
                if (visitEdge[edge])
                    continue;
                visitEdge[edge] = true;
                for (auto &w : hyperEdge[edge])
                {
                    if (minK == core.at(w) && visitNode.find(w) == visitNode.end())
                    {
                        Q.push(w);
                        visitNode[w] = true;
                    }
                }
            }
        }
    }
}

void insertEdge(vector<vector<int>> &hyperEdge, unordered_map<int, vector<int>> &hyperNode, unordered_map<int, int> &core, const vector<int> &e, int index)
{
    //插入超边到超图
    for (auto &it : e)
    {
        hyperEdge[index].push_back(it);
        hyperNode[it].push_back(index);
    }

    int r = e[0];
    bool flag = false;
    for (int i = 0; i < e.size(); i++)
    {
        if (core.find(e[i]) == core.end())
        {
            core[e[i]] = 1;
            flag = true;
        }
        if (core[r] > core[e[i]])
            r = e[i];
    }
    if (flag)
    {
        return;
    }
    unordered_map<int, int> cd;
    unordered_map<int, vector<int>> preEdge;
    findHypergraphSubCore1(hyperEdge, hyperNode, core, r, preEdge, cd);
    int k = core.at(r);
    set<pair<int, int>, myCmp> mcd;
    unordered_map<int, int> deg;
    for (auto &node : cd)
    {
        if (core[node.first] == k)
        {
            mcd.insert(make_pair(node.first, node.second));
            deg[node.first] = node.second;
        }
    }
    vector<bool> visitEdge(hyperEdge.size(), false);
    while (!mcd.empty() && (*mcd.begin()).second <= k)
    {
        pair<int, int> p = *mcd.begin();
        mcd.erase(mcd.begin());
        int v = p.first;
        for (auto &edge : preEdge[v])
        {
            if (visitEdge[edge])
                continue;
            visitEdge[edge] = true;
            for (auto &node : hyperEdge[edge])
            {
                if (deg.find(node) == deg.end() || deg.at(node) <= deg.at(v))
                    continue;
                mcd.erase(make_pair(node, deg[node]));
                deg[node]--;
                mcd.insert(make_pair(node, deg[node]));
            }
        }
        deg.erase(v);
    }
    while (!mcd.empty())
    {
        pair<int, int> p = *mcd.begin();
        mcd.erase(mcd.begin());
        core[p.first]++;
    }
}

void kcoreDecomp(vector<vector<int>> &hyperEdge, unordered_map<int, vector<int>> &hyperNode, unordered_map<int, int> &core)
{
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
                if (visitNode[node])
                    continue;
                node_count.erase(make_pair(node, deg[node]));
                deg[node]--;
                node_count.insert(make_pair(node, deg[node]));
            }
        }
    }
}

void findHypergraphSubCore(const vector<vector<int>> &hyperEdge, const unordered_map<int, vector<int>> &hyperNode, const unordered_map<int, int> &core, vector<int> &u,
                           unordered_map<int, vector<int>> &preEdge, unordered_map<int, int> &cd)
{
    unordered_map<int, bool> visitNode;
    queue<int> Q;
    int k = core.at(u[0]);
    for (auto node : u)
    {
        Q.push(node);
        visitNode[node] = true;
    }
    vector<int> visitEdge(hyperEdge.size(), false);
    while (!Q.empty())
    {
        int v = Q.front();
        Q.pop();

        for (auto &edge : hyperNode.at(v))
        {
            int minK = INT_MAX;
            for (auto &w : hyperEdge[edge])
                minK = min(minK, core.at(w));
            if (minK == k)
            {
                cd[v]++;
                preEdge[v].push_back(edge);
                if (visitEdge[edge])
                    continue;
                visitEdge[edge] = true;
                for (auto &w : hyperEdge[edge])
                {
                    if (minK == core.at(w) && visitNode.find(w) == visitNode.end())
                    {
                        Q.push(w);
                        visitNode[w] = true;
                    }
                }
            }
        }
    }
}

void eraseEdge(vector<vector<int>> &hyperEdge, unordered_map<int, vector<int>> &hyperNode, unordered_map<int, int> &core, int index)
{
    vector<int> e;
    hyperEdge[index].swap(e);
    hyperEdge[index].clear();
    for (auto node : e)
    {
        hyperNode[node].erase(find(hyperNode[node].begin(), hyperNode[node].end(), index));
    }
    int r = e[0];
    vector<int> rr;
    for (int i = 0; i < int(e.size()); i++)
    {
        if (core[r] > core[e[i]])
            r = e[i];
    }
    for (int i = 0; i < int(e.size()); i++)
    {
        if (core[r] == core[e[i]])
            rr.push_back(e[i]);
    }
    unordered_map<int, int> cd;
    unordered_map<int, vector<int>> preEdge;
    findHypergraphSubCore(hyperEdge, hyperNode, core, rr, preEdge, cd);
    int k = core.at(r);
    set<pair<int, int>, myCmp> mcd;
    unordered_map<int, int> deg;
    for (auto &node : cd)
    {
        if (core[node.first] == k)
        {
            mcd.insert(make_pair(node.first, node.second));
            deg[node.first] = node.second;
        }
    }
    vector<bool> visitEdge(hyperEdge.size(), false);
    while (!mcd.empty() && (*mcd.begin()).second < k)
    {
        pair<int, int> p = *mcd.begin();
        mcd.erase(mcd.begin());
        int v = p.first;
        core[v]--;
        for (auto &edge : preEdge[v])
        {
            if (visitEdge[edge])
                continue;
            visitEdge[edge] = true;
            for (auto &node : hyperEdge[edge])
            {
                if (deg.find(node) == deg.end() || deg.at(node) <= deg.at(v))
                    continue;
                mcd.erase(make_pair(node, deg[node]));
                deg[node]--;
                mcd.insert(make_pair(node, deg[node]));
            }
        }
        deg.erase(v);
    }
}

void find(int k, vector<int> &coreE, vector<int> &edge)
{
    int count = 0;
    for (int i = 0; i < int(coreE.size()); i++)
    {
        if (coreE[i] == k)
        {
            edge.push_back(i);
            ++count;
            if (count == 50)
                return;
        }
    }
}

void getcore(unordered_map<int, int> &core, vector<int> &coreE, const string &str)
{
    ifstream fin("./result/CoreNumber/" + str + "-coreVertexValue");
    int a, b;
    while (fin >> a >> b)
        core[a] = b;
    fin.close();
    fin.open("./result/CoreNumber/" + str + "-coreEdgeValue");
    while (fin >> a)
        coreE.push_back(a);
    fin.close();
}

void solve(const string &str, const int &increase, const int &end)
{
    ofstream fout1("./result/timeRelatedToK/" + str + "-insert");
    ofstream fout2("./result/timeRelatedToK/" + str + "-erase");
    string filepath = "/home/liuyu/C++Projects/graphData/hypergraph/";
    string filename = filepath + str;
    vector<vector<int>> hyperEdge;
    unordered_map<int, vector<int>> hyperNode;
    unordered_map<int, int> core;
    vector<int> coreE;
    vector<vector<int>> InsertEdge;
    getGraph(filename, hyperEdge, hyperNode);
    getcore(core, coreE, str);
    set<int> t;
    t.insert(1);
    int count = 1;
    while (count * increase <= end)
    {
        t.insert(count * increase);
        count++;
    }
    for (auto k : t)
    {
        fout1 << k << ' ';
        fout2 << k << ' ';
        vector<int> edge;
        find(k, coreE, edge);
        for (auto index : edge)
        {
            vector<int> e(hyperEdge[index]);
            auto t1 = std::chrono::steady_clock::now();
            eraseEdge(hyperEdge, hyperNode, core, index);
            auto t2 = std::chrono::steady_clock::now();
            insertEdge(hyperEdge, hyperNode, core, e, index);
            auto t3 = std::chrono::steady_clock::now();
            double dr_ns1 = std::chrono::duration<double, std::nano>(t2 - t1).count();
            double dr_ns2 = std::chrono::duration<double, std::nano>(t3 - t2).count();
            fout1 << dr_ns2 << ' ';
            fout2 << dr_ns1 << ' ';
        }
        fout1 << endl;
        fout2 << endl;
    }
    fout1.close();
    fout2.close();
    cout << "pid=" << std::this_thread::get_id() << ' ' << str << " finish!" << endl;
}

int main()
{
    // string file = "S_CoGe  S_CoHi  S_DAWN  S_DrAb  S_MiAc  S_NDC  S_TrCl  S_WaTr";
    string file = "MaAn ThAU ThMS CoMH";
    string in = "1 1 1 1";
    string en = "2 25 93 1077";    
    istringstream ss1(file);
    istringstream ss2(in);
    istringstream ss3(en);
    string str;
    int increase;
    int end;
    std::vector<std::thread> thread_vec;
    while (ss1 >> str)
    {
        ss2 >> increase;
        ss3 >> end;
        thread_vec.emplace_back(std::thread(solve, str, increase, end));
    }
    for (auto &it : thread_vec)
    {
        it.join();
    }
    cout << "全部完成" << endl;
}