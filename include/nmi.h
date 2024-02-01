//
// Created by yming on 1/29/24.
//

#ifndef BI_GRAPH_CLUSTER_NMI_H
#define BI_GRAPH_CLUSTER_NMI_H
#include "utils.h"

using namespace std;
int getNodesNum(vector<vector<int> > & X, vector<vector<int> > & Y)
{
    set<int> s;
    for (size_t i = 0; i < X.size(); ++i)
    {
        for (size_t j = 0; j < X[i].size(); ++j)
            s.insert(X[i][j]);
    }
    for (size_t i = 0; i < Y.size(); ++i)
    {
        for (size_t j = 0; j < Y[i].size(); ++j)
            s.insert(Y[i][j]);
    }
    return s.size();
}

double g_nodes_num;

double log2(double x)
{
    return log(x) / log(2);
}
double h(double x)
{
    if (x > 0)
        return -1 * x * log2(x);
    else
        return 0;
}
double H(vector<int> & Xi)
{
    double p1 = Xi.size() / g_nodes_num;
    double p0 = 1 - p1;
    return h(p0) + h(p1);
}

vector<int> intersection(vector<int>& a, vector<int>& b)
{
    sort(a.begin(), a.end());
    sort(b.begin(), b.end());

    vector<int> res(max(a.size(), b.size()));
    auto iter = std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), res.begin());
    res.resize(iter - res.begin());

    return res;
}

vector<int> difference(vector<int>& a, vector<int>& b)
{
    sort(a.begin(), a.end());
    sort(b.begin(), b.end());

    vector<int> res(max(a.size(), b.size()));
    auto iter = std::set_difference(a.begin(), a.end(), b.begin(), b.end(), res.begin());
    res.resize(iter - res.begin());

    return res;
}
//H(Xi, Yj)
double H_Xi_joint_Yj(vector<int> & Xi, vector<int> & Yj)
{
    double P11 = intersection(Xi, Yj).size() / g_nodes_num;
    double P10 = difference(Xi, Yj).size() / g_nodes_num;
    double P01 = difference(Yj, Xi).size() / g_nodes_num;
    double P00 = 1 - P11 - P10 - P01;
    return h(P11) + h(P10) + h(P01) + h(P00);
}

double H_Xi_given_Yj(vector<int> & Xi, vector<int> & Yj)
{
    return H_Xi_joint_Yj(Xi, Yj) - H(Yj);
}

double H_Xi_given_Y(vector<int> & Xi, vector<vector<int> > & Y)
{
    double res = H_Xi_given_Yj(Xi, Y[0]);
    for (size_t i = 1; i < Y.size(); ++i)
    {
        res = min(res, H_Xi_given_Yj(Xi, Y[i]));
    }
    return res;
}

double H_Xi_given_Y_norm(vector<int> & Xi, vector<vector<int> > & Y)
{
    return H_Xi_given_Y(Xi, Y) / H(Xi);
}

double H_X_given_Y(vector<vector<int> > & X, vector<vector<int> > & Y)
{
    double res = 0;
    for (size_t i = 0; i < X.size(); ++i)
    {
        res += H_Xi_given_Y(X[i], Y);
    }

    return res;
}

//H(X|Y)_norm
double H_X_given_Y_norm(vector<vector<int> > & X, vector<vector<int> > & Y)
{
    double res = 0;
    for (size_t i = 0; i < X.size(); ++i)
    {
        res += H_Xi_given_Y_norm(X[i], Y);
    }

    return res / X.size();
}

double NMI(vector<vector<int> > & X, vector<vector<int> > & Y)
{
    if (X.size() == 0 || Y.size() == 0)
        return 0;
    g_nodes_num = getNodesNum(X, Y);

    return 1 - 0.5 * (H_X_given_Y_norm(X, Y) + H_X_given_Y_norm(Y, X));
}

#endif //BI_GRAPH_CLUSTER_NMI_H
