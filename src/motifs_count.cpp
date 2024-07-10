#include "graph.h"

LL Graph::exact_neighbor_intersections(int u, int v) {
    LL common = 0;
    if (graph_[u].size() > graph_[v].size()) std::swap(u, v);
    std::unordered_set<int> set;
    for (auto neighbor: graph_[u]) set.insert(neighbor);
    for (auto neighbor: graph_[v]) {
        if (set.find(neighbor) != set.end())
            common++;
    }
    return common;
}

LL Graph::naive_compute_common_bflys(int u, int v) {
    LL bfc_per_edge = 0;
    for (auto neighbor: graph_[u]) {
        if (neighbor != v) {
            bfc_per_edge += exact_neighbor_intersections(neighbor, v) - 1;
        }
    }
    return bfc_per_edge;
}

LL Graph::fast_compute_common_bflys(int u, int v, std::unordered_map<int, int> &index_map) {
    LL bfc_per_edge = 1;
    for (auto neighbor: graph_[v]) {
        bfc_per_edge += index_map[neighbor];
    }
    bfc_per_edge -= (LL) graph_[v].size();
    bfc_per_edge -= (LL) graph_[u].size();
    return bfc_per_edge;
}


void Graph::get_two_hop_map(int u, std::unordered_map<int, int> &index_map) {
    for (auto neighbor: graph_[u]) {
        for (auto two_hop_neighborhood: graph_[neighbor]) {
            index_map[two_hop_neighborhood]++;
        }
    }
}

void Graph::get_two_hop_count(int u) {
    node_two_hop_[u] = 0;
    for (auto neighbor: graph_[u]) {
        node_two_hop_[u] += (LL)graph_[neighbor].size();
    }
    node_two_hop_[u] -= (LL)graph_[u].size();
}