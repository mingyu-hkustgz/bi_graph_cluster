#include "graph.h"

void Graph::dynamic_index_init(char *filename) {
    std::ifstream fin(filename, std::ios::binary);
    fin.read((char *) &node_num, sizeof(int));
    fin.read((char *) &edge_num, sizeof(int));
    fin.read((char *) &left_nodes, sizeof(int));
    fin.read((char *) &right_nodes, sizeof(int));
    fin.read((char *) &max_degree_, sizeof(int));
    dynamic_bm_.resize(node_num, false);
    nbr_order_ = new std::vector<float>[node_num];
    nbr_ = new std::unordered_map<int, int>[node_num];
    nbr_mp_.resize(node_num, std::multimap<float, int, std::greater<>>());
    cores_mp_left_.resize(max_degree_ + 1, std::multimap<float, int, std::greater<>>());
    cores_mp_right_.resize(max_degree_ + 1, std::multimap<float, int, std::greater<>>());
    graph_.resize(node_num);
    for (int i = 0; i < node_num; i++) {
        unsigned size;
        fin.read((char *) &size, sizeof(unsigned));
        graph_[i].resize(size);
        fin.read((char *) graph_[i].data(), sizeof(int) * size);
    }
    for (int i = 0; i < node_num; i++) {
        for (auto v: graph_[i]) {
            float similarity;
            fin.read((char *) &similarity, sizeof(float));
            similarity_square_[std::make_pair(i, v)] = similarity;
            similarity_square_[std::make_pair(v, i)] = similarity;
            nbr_order_[i].push_back(similarity);
            nbr_mp_[i].emplace(similarity, v);
        }
    }
    for (int i = 0; i < node_num; i++) {
        for (auto v: graph_[i]) {
            int com_fly;
            fin.read((char *) &com_fly, sizeof(int));
            common_bflys_[std::make_pair(i, v)] = com_fly;
            common_bflys_[std::make_pair(v, i)] = com_fly;
            nbr_[i].emplace(v, com_fly);
        }
    }
    index_core_cnt_left.resize(max_degree_ + 1);
    index_core_cnt_right.resize(max_degree_ + 1);
    fin.read((char *) index_core_cnt_left.data(), sizeof(int) * (max_degree_ + 1));
    fin.read((char *) index_core_cnt_right.data(), sizeof(int) * (max_degree_ + 1));
    index_core_left.resize(max_degree_ + 1);
    index_core_right.resize(max_degree_ + 1);
    for (int i = 1; i <= max_degree_; i++) {
        index_core_left[i].resize(index_core_cnt_left[i]);
        index_core_right[i].resize(index_core_cnt_right[i]);
        fin.read((char *) index_core_left[i].data(), sizeof(int) * index_core_cnt_left[i]);
        fin.read((char *) index_core_right[i].data(), sizeof(int) * index_core_cnt_right[i]);
        for (int j = 0; j < index_core_cnt_left[i]; j++) {
            unsigned id = index_core_left[i][j];
            cores_mp_left_[i].emplace(nbr_order_[id][i - 1], id);
        }
        for (int j = 0; j < index_core_cnt_right[i]; j++) {
            unsigned id = index_core_right[i][j];
            cores_mp_right_[i].emplace(nbr_order_[id][i - 1], id);
        }
    }
}


void Graph::recompute_edge_similarity(int u, int v) {
    LL uv_wedge = ((LL) graph_[u].size() - 1) * ((LL) graph_[v].size() - 1) + 1;
    LL cn = common_bflys_[std::make_pair(u, v)];
    float tmp_eps = (long double) (cn + 1) / uv_wedge;
    similarity_square_[std::make_pair(u, v)] = tmp_eps;
    similarity_square_[std::make_pair(v, u)] = tmp_eps;
}



void Graph::naive_insert_edge(int u, int v) {
    int left = std::min(u, v), right = std::max(u, v);
    if (left > left_nodes || right <= left_nodes) return;
    for (auto neighbor: graph_[u]) {
        if (neighbor == v) return;
    }
    int new_btf = 0;
    std::unordered_map<int, int> two_hop_map;
    get_two_hop_map(u, two_hop_map);
    new_btf = fast_compute_common_bflys(u, v, two_hop_map);
    common_bflys_[std::make_pair(u, v)] = new_btf;
    common_bflys_[std::make_pair(v, u)] = new_btf;
    boost::unordered_map<std::pair<int, int>, bool, boost::hash<std::pair<int, int>>> common_edges_;
    for (auto neighbor: graph_[u]) {
        for (auto two_hop: graph_[neighbor]) {
            if (two_hop != u) {
                common_edges_[std::make_pair(neighbor, two_hop)] = true;
                common_edges_[std::make_pair(two_hop, neighbor)] = true;
            }
        }
    }
    for (auto next_u: graph_[u]) {
        for (auto next_v: graph_[v]) {
            if (common_edges_[std::make_pair(next_u, next_v)]) {
                common_bflys_[std::make_pair(next_u, next_v)]++;
                common_bflys_[std::make_pair(next_v, next_u)]++;
                common_bflys_[std::make_pair(u, next_u)]++;
                common_bflys_[std::make_pair(next_u, u)]++;
                common_bflys_[std::make_pair(next_v, v)]++;
            }

        }
    }
    graph_[u].push_back(v);
    graph_[v].push_back(u);
    for (auto neighbor: graph_[u]) {
        recompute_edge_similarity(neighbor, u);
        for (auto two_hop: graph_[neighbor]) {
            recompute_edge_similarity(neighbor, two_hop);
        }
    }
    for (auto neighbor: graph_[v]) {
        recompute_edge_similarity(neighbor, v);
        for (auto two_hop: graph_[neighbor]) {
            recompute_edge_similarity(neighbor, two_hop);
        }
    }
}

void Graph::naive_delete_edge(int u, int v) {
    int left = std::min(u, v), right = std::max(u, v);
    if (left > left_nodes || right <= left_nodes) return;
    bool check = false;
    for (auto neighbor: graph_[u]) {
        if (neighbor == v) {
            check = true;
            break;
        }
    }
    if (!check) return;
    int del_btf = 0;
    std::unordered_map<int, int> two_hop_map;
    get_two_hop_map(u, two_hop_map);
    del_btf = fast_compute_common_bflys(u, v, two_hop_map);
}