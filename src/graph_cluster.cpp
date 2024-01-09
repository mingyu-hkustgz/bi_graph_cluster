//
// Created by Mingyu on 24-1-7.
//
#include "graph.h"


void Graph::naive_cluster_construct() {
    for (int u = 0; u < node_num; u++) {
        if (u % (node_num / 100 + 1) == 0) std::cerr << "current :: tag -> :: " << u << std::endl;

#ifdef HASHMAP
        std::unordered_map<int, int> two_hop_map;
        get_two_hop_map(u, two_hop_map);
#endif
        if (node_two_hop_[u] == -1) get_two_hop_count(u);
        for (auto v: graph_[u]) {
            if (v < u) continue;
            if (node_two_hop_[v] == -1) get_two_hop_count(v);
            LL v_wedge = node_two_hop_[v] - (LL) graph_[u].size() + 2;
            LL u_wedge = node_two_hop_[u] - (LL) graph_[v].size() + 2;
            LL uv_wedge = ((LL) graph_[u].size() - 1) * ((LL) graph_[v].size() - 1) + 1;
            int cn = 0;
#ifdef HASHMAP
            cn = fast_compute_common_bflys(u, v, two_hop_map);
#else
            cn = naive_compute_common_bflys(u,v);
#endif
            common_bflys_[std::make_pair(u, v)] = cn;
            common_bflys_[std::make_pair(v, u)] = cn;
            float tmp_eps =
                    (long double) (cn + 1) / std::pow((long double) (u_wedge * v_wedge * uv_wedge), 1.0 / 3.0);
            similarity_square_[std::make_pair(u, v)] = tmp_eps;
            similarity_square_[std::make_pair(v, u)] = tmp_eps;
        }
    }
}

void Graph::degree_cluster_construct() {
    int *head = new int[max_degree_ + 1];
    for (int i = 0; i <= max_degree_; ++i) head[i] = -1;
    int *next = new int[node_num];
    for (int i = 0; i < node_num; ++i) {
        int degree = (int) graph_[i].size() - 1;
        next[i] = head[degree];
        head[degree] = i;
    }
    boost::dynamic_bitset<> pru{node_num, 0};
    boost::dynamic_bitset<> cn_bm_{node_num, 0};
    int cd = max_degree_;
    int u = head[cd];
    while (cd > 0) {
        if (u == -1) {
            u = head[--cd];
            continue;
        }
        for (auto neighbor: graph_[u])
            cn_bm_.set(neighbor, true);
#ifdef HASHMAP
        std::unordered_map<int, int> two_hop_map;
        get_two_hop_map(u, two_hop_map);
#endif
        if (node_two_hop_[u] == -1) get_two_hop_count(u);
        for (auto v: graph_[u]) {
            if (pru[v]) continue;
            if (node_two_hop_[v] == -1) get_two_hop_count(v);
            LL v_wedge = node_two_hop_[v] - (LL) graph_[u].size() + 2;
            LL u_wedge = node_two_hop_[u] - (LL) graph_[v].size() + 2;
            LL uv_wedge = ((LL) graph_[u].size() - 1) * ((LL) graph_[v].size() - 1) + 1;
            int cn = 0;
#ifdef HASHMAP
            cn = fast_compute_common_bflys(u, v, two_hop_map);
#else
            cn = naive_compute_common_bflys(u,v);
#endif
            common_bflys_[std::make_pair(u, v)] = cn;
            common_bflys_[std::make_pair(v, u)] = cn;
            float tmp_eps =
                    (long double) (cn + 1) / std::pow((long double) (u_wedge * v_wedge * uv_wedge), 1.0 / 3.0);
            similarity_square_[std::make_pair(u, v)] = tmp_eps;
            similarity_square_[std::make_pair(v, u)] = tmp_eps;
        }
        for (auto neighbor: graph_[u])
            cn_bm_.set(neighbor, false);

        pru[u] = true;
        u = next[u];
    }
    delete[] head;
    delete[] next;
}

void Graph::sort_nbr_by_similarity(int u) {
    auto &tmp_square = similarity_square_;
    std::sort(graph_[u].begin(), graph_[u].end(), [&tmp_square, u](int a, int b) {
        return tmp_square[std::make_pair(u, a)] > tmp_square[std::make_pair(u, b)];
    });
}


void Graph::sort_cores(int eps_deg) {
    int cur_left = 0, cur_right = 0;
    if (eps_deg == 1) {
        // for(int i = 0; i < n_; ++i) idx1_cores_[eps_deg][i] = i;
        for (int i = 0; i <= left_nodes; ++i) {
            if (graph_[i].size() < eps_deg) continue;
            index_core_left[eps_deg][cur_left++] = i;
        }
        for (int i = left_nodes + 1; i < node_num; ++i) {
            if (graph_[i].size() < eps_deg) continue;
            index_core_right[eps_deg][cur_right++] = i;
        }
    } else {
        for (int i = 0; i < index_core_cnt_left[eps_deg - 1]; ++i) {
            int id = index_core_left[eps_deg - 1][i];
            if (graph_[id].size() < eps_deg) continue;
            index_core_left[eps_deg][cur_left++] = id;
        }
        for (int i = 0; i < index_core_cnt_right[eps_deg - 1]; ++i) {
            int id = index_core_right[eps_deg - 1][i];
            if (graph_[id].size() < eps_deg) continue;
            index_core_right[eps_deg][cur_right++] = id;
        }
    }
    auto &tmp_square = similarity_square_;
    auto &tmp_graph = graph_;
    std::sort(index_core_left[eps_deg].begin(), index_core_left[eps_deg].end(),
              [eps_deg, &tmp_square, &tmp_graph](int u, int v) {
                  return tmp_square[std::make_pair(u, tmp_graph[u][eps_deg - 1])] >
                         tmp_square[std::make_pair(v, tmp_graph[v][eps_deg - 1])];
              });
    std::sort(index_core_right[eps_deg].begin(), index_core_right[eps_deg].end(),
              [eps_deg, &tmp_square, &tmp_graph](int u, int v) {
                  return tmp_square[std::make_pair(u, tmp_graph[u][eps_deg - 1])] >
                         tmp_square[std::make_pair(v, tmp_graph[v][eps_deg - 1])];
              });

}

void Graph::index_cluster_construct() {
    index_core_cnt_left.resize(max_degree_ + 1, 0);
    index_core_cnt_right.resize(max_degree_ + 1, 0);
    for (int u = 0; u < node_num; ++u) {
        if (u % (node_num / 100 + 1) == 0) std::cerr << "current :: tag -> :: " << u << std::endl;
        // compute similarity of all edges connected to u
#ifdef HASHMAP
        std::unordered_map<int, int> two_hop_map;
        get_two_hop_map(u, two_hop_map);
#endif
        if (node_two_hop_[u] == -1) get_two_hop_count(u);
        for (auto v: graph_[u]) {
            if (v < u) continue;
            if (node_two_hop_[v] == -1) get_two_hop_count(v);
            LL v_wedge = node_two_hop_[v] - (LL) graph_[u].size() + 2;
            LL u_wedge = node_two_hop_[u] - (LL) graph_[v].size() + 2;
            LL uv_wedge = ((LL) graph_[u].size() - 1) * ((LL) graph_[v].size() - 1) + 1;
            int cn = 0;
#ifdef HASHMAP
            cn = fast_compute_common_bflys(u, v, two_hop_map);
#else
            cn = naive_compute_common_bflys(u,v);
#endif
            common_bflys_[std::make_pair(u, v)] = cn;
            common_bflys_[std::make_pair(v, u)] = cn;
            float tmp_eps =
                    (long double) (cn + 1) / std::pow((long double) (u_wedge * v_wedge * uv_wedge), 1.0 / 3.0);
            similarity_square_[std::make_pair(u, v)] = tmp_eps;
            similarity_square_[std::make_pair(v, u)] = tmp_eps;
        }
        if (u <= left_nodes)
            ++index_core_cnt_left[graph_[u].size()];
        else
            ++index_core_cnt_right[graph_[u].size()];
        sort_nbr_by_similarity(u);
        // quick_sort_nbr_by_similarity(u, offset_[u], offset_[u+1]);
    }
    std::cerr << "sort core order" << std::endl;

    // compute the number of vertices under each miu
    for (int i = max_degree_ - 1; i > 0; --i) {
        index_core_cnt_left[i] += index_core_cnt_left[i + 1];
        index_core_cnt_right[i] += index_core_cnt_right[i + 1];
    }

    index_core_left.resize(max_degree_ + 1);
    index_core_right.resize(max_degree_ + 1);
    // construct the hierarchy
    for (int i = 1; i <= max_degree_; ++i) {
        index_core_left[i].resize(index_core_cnt_left[i]);
        index_core_right[i].resize(index_core_cnt_right[i]);
        sort_cores(i);
    }
}
