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

int main()
{
    // string file = "S_CoGe  S_CoHi  S_DAWN  S_DrAb  S_MaAn  S_MiAc  S_NDC  S_StAn  S_TrCl  S_WaTr T_CoBi T_CoDB T_ThAU T_ThSO";
    string file ="MaAn ThAU ThMS CoMH";
    string filepath = "/home/liuyu/C++Projects/graphData/hypergraph/";
    istringstream ss(file);
    string str;
    ofstream fout("./result/dataSet-info.txt");
    while (ss>>str)
    {
        string filename = filepath + str;
        vector<vector<int>> hyperEdge;
        unordered_map<int, vector<int>> hyperNode;
        getGraph(filename, hyperEdge, hyperNode);
        int deg_max=INT_MIN;
        int deg_count=0;
        int cardi_max=INT_MIN;
        int cardi_count=0;
        for(auto &it:hyperNode)
        {
            deg_max=max(deg_max,int(it.second.size()));
            deg_count+=int(it.second.size());
        }
        for(auto &it:hyperEdge)
        {
            cardi_max=max(cardi_max,int(it.size()));
            cardi_count+=int(it.size());
        }
        fout<<str<<' '<<int(hyperNode.size())<<' ' <<int(hyperEdge.size())<<' '<<deg_max<<' '<<deg_count/int(hyperNode.size())<<' '<<cardi_max<<' '<<cardi_count/int(hyperEdge.size())<<endl;
        cout<<str<<endl;
    }
}