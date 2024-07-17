#include "utils.h"

bool isFileExists_ifstream(const char *name) {
    std::ifstream f(name);
    return f.good();
}

double compute_modularity(std::vector<std::vector<int>> G, std::unordered_map<int, int> partition,
                          const std::string& cluster_type) {

    std::vector<std::vector<int>> tmp_graph(G.size());
    for (int i = 0; i < G.size(); i++) {
        if (partition[i] == -1 && cluster_type != "origin") continue;
        for (int j: G[i]) {
            if (partition[j] == -1 && cluster_type != "origin") continue;
            tmp_graph[i].push_back(j);
        }
    }

    std::unordered_map<int, double> inc, deg;
    double links = 0;
    for (int node = 0; node < tmp_graph.size(); node++) {
        links += (double) tmp_graph[node].size() / 2;
        auto com = partition[node];
        deg[com] = deg[com] + (double) tmp_graph[node].size();
        for (auto neighbor: tmp_graph[node]) {
            if (partition[neighbor] == com) {
                if (neighbor == node)
                    inc[com] = inc[com] + 1;
                else
                    inc[com] = inc[com] + 0.5;
            }
        }
    }
    double res = 0;
    std::set<int> partition_value;
    for (auto com: partition) {
        partition_value.insert(com.second);
    }
    for (auto com: partition_value) {
        double C1 = inc[com] / links;
        double C2 = deg[com] / (2.0 * links);
        res += (C1 - C2 * C2);
    }
    return res;
}