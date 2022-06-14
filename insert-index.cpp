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

void findHypergraphSubCore(const vector<vector<int>> &hyperEdge, const unordered_map<int, unordered_set<int>> &nodeinfo, const unordered_map<int, int> &core, int u,
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

        for (auto &edge : nodeinfo.at(v))
        {
            cd[v]++;
            preEdge[v].push_back(edge);
            if (visitEdge[edge])
                continue;
            visitEdge[edge] = true;
            for (auto &w : hyperEdge[edge])
            {
                if (k == core.at(w) && visitNode.find(w) == visitNode.end())
                {
                    Q.push(w);
                    visitNode[w] = true;
                }
            }
        }
    }
}

void insertEdge(vector<vector<int>> &hyperEdge, unordered_map<int, vector<int>> &hyperNode, unordered_map<int, int> &core, const vector<int> &e, unordered_map<int, unordered_set<int>> &nodeinfo, vector<int> &edgeinfo)
{

    //更新超图
    int index = int(hyperEdge.size());
    hyperEdge.emplace_back(vector<int>());
    for (auto node : e)
    {
        hyperEdge[index].emplace_back(node);
        hyperNode[node].emplace_back(index);
    }
    //计算插入的边的最小kcore和最小core值集合
    int mink_e = INT_MAX;
    vector<int> mink_e_node;
    bool flag_isNewNode = false;
    for (auto node : e)
    {
        if (core[node] == 0)
        {
            core[node] = 1;
            flag_isNewNode = true;
        }
        if (mink_e > core[node])
        {
            mink_e = core[node];
            mink_e_node.clear();
            mink_e_node.emplace_back(node);
        }
        else if (mink_e == core[node])
        {
            mink_e_node.emplace_back(node);
        }
    }
    //更新节点和边信息
    //对最小节点集合内的节点均需要增加一条为e的边
    for (auto node : mink_e_node)
    {
        nodeinfo[node].insert(index);
    }
    edgeinfo.emplace_back(mink_e);
    if (flag_isNewNode)
        return;
    int r = mink_e_node[0];
    unordered_map<int, int> cd;
    unordered_map<int, vector<int>> preEdge;
    findHypergraphSubCore(hyperEdge, nodeinfo, core, r, preEdge, cd);
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
    vector<int> tmp_node;
    while (!mcd.empty())
    {
        pair<int, int> p = *mcd.begin();
        mcd.erase(mcd.begin());
        core[p.first]++;
        tmp_node.emplace_back(p.first);
    }
    //更新对应边的core值
    for (auto node : tmp_node)
    {
        vector<int> erase;
        for (auto edge : nodeinfo[node])
        {
            int mink = INT_MAX;
            for (auto node : hyperEdge[edge])
            {
                mink = min(mink, core.at(node));
            }
            edgeinfo[edge] = mink;
            if (mink <= mink_e)
                erase.emplace_back(edge);
        }
        for (auto edge : erase)
        {
            nodeinfo[node].erase(edge);
        }
    }
    for (auto node : tmp_node)
    {
        for (auto edge : nodeinfo[node])
            for (auto node1 : hyperEdge[edge])
                if (core[node1] == edgeinfo[edge])
                {
                    if (nodeinfo[node1].find(edge) == nodeinfo[node1].end())
                    {
                        nodeinfo[node1].insert(edge);
                    }
                }
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
void initial(vector<vector<int>> &hyperEdge, unordered_map<int, vector<int>> &hyperNode)
{
    hyperNode.clear();
    for (int i = 0; i < int(hyperEdge.size()); i++)
    {
        for (auto node : hyperEdge[i])
            hyperNode[node].emplace_back(i);
    }
}
void dividDataSet(vector<vector<int>> &hyperEdge, vector<vector<int>> &InsertEdge, int n)
{
    for (int i = 0; i < n; i++)
    {
        InsertEdge.emplace_back(hyperEdge[i]);
    }
    hyperEdge.erase(hyperEdge.begin(), hyperEdge.begin() + n);
}

void initialization(const vector<vector<int>> &hyperEdge, const unordered_map<int, vector<int>> &hyperNode, const unordered_map<int, int> &core, unordered_map<int, unordered_set<int>> &nodeinfo, vector<int> &edgeinfo)
{
    for (int i = 0; i < int(hyperEdge.size()); i++)
    {
        int mink = INT_MAX;
        for (auto node : hyperEdge[i])
            mink = min(mink, core.at(node));
        edgeinfo[i] = mink;
    }
    for (auto &it : hyperNode)
    {
        for (auto edge : it.second)
        {
            if (edgeinfo[edge] == core.at(it.first))
                nodeinfo[it.first].insert(edge);
        }
    }
}

void solve(const string &str)
{
    ofstream fout("./result/insertTime/" + str + "-insetTime-index");
    string filepath = "/home/liuyu/C++Projects/graphData/hypergraph/synthetic/";
    for (int i = 0; i < 5; i++)
    {
        cout<<str<<' '<<i<<endl;
        string filename = filepath + str;
        vector<vector<int>> hyperEdge;
        unordered_map<int, vector<int>> hyperNode;
        unordered_map<int, int> core;
        vector<vector<int>> InsertEdge;
        getGraph(filename, hyperEdge, hyperNode);
        int num = 5000;
        mt19937 g(i);
        shuffle(hyperEdge.begin(), hyperEdge.end(), g);
        dividDataSet(hyperEdge, InsertEdge, num);
        initial(hyperEdge, hyperNode);
        kcoreDecomp(hyperEdge, hyperNode, core);
        unordered_map<int, unordered_set<int>> nodeinfo;
        vector<int> edgeinfo(int(hyperEdge.size()));
        initialization(hyperEdge, hyperNode, core, nodeinfo, edgeinfo);
        auto t5 = std::chrono::steady_clock::now();
        for (int j = 0; j < num; j++)
        {
            insertEdge(hyperEdge, hyperNode, core, InsertEdge[i], nodeinfo, edgeinfo);
        }
        auto t6 = std::chrono::steady_clock::now();
        auto dr_ns3 = std::chrono::duration<double, std::nano>(t6 - t5).count();
        fout<<dr_ns3<<endl;

    }
    fout.close();
    cout << "pid=" << std::this_thread::get_id() << ' ' << str << " finish!" << endl;
}

int main()
{
    string file = "CoMH-1";
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