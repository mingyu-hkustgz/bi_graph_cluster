#include "graph.h"


void Graph::naive_cluster_construct(bool use_hash) {
    for (int u = 0; u < node_num; u++) {
        if (u % (node_num / 100 + 1) == 0 && node_num > 1000) std::cerr << "current :: tag -> :: " << u << std::endl;
        std::unordered_map<int, int> two_hop_map;
        if (use_hash) {
            get_two_hop_map(u, two_hop_map);
        }
        for (auto v: graph_[u]) {
            if (v < u) continue;
            LL uv_wedge = ((LL) graph_[u].size() - 1) * ((LL) graph_[v].size() - 1) + 1;
            LL cn = 0;
            if (use_hash)
                cn = fast_compute_common_bflys(u, v, two_hop_map);
            else
                cn = naive_compute_common_bflys(u, v);
            common_bflys_[std::make_pair(u, v)] = cn;
            common_bflys_[std::make_pair(v, u)] = cn;
            long double base_eps = (long double) (cn + 1) / (long double) (uv_wedge);
            float tmp_eps = base_eps;
            similarity_square_[std::make_pair(u, v)] = tmp_eps;
            similarity_square_[std::make_pair(v, u)] = tmp_eps;
        }
    }
}

void Graph::naive_parallel_cluster_construct(int threads) {
    unsigned check_tag = 0;
    check_tag = 0;
#pragma omp parallel for num_threads(threads)
    for (int u = 0; u < node_num; u++) {
#pragma omp critical
        {
            check_tag++;
            if (check_tag % (node_num / 100 + 1) == 0 && node_num > 1000)
                std::cerr << "current tag:: " << check_tag << std::endl;
        }
        std::unordered_map<int, int> two_hop_map;
        get_two_hop_map(u, two_hop_map);
        for (auto v: graph_[u]) {
            if (v < u) continue;
            LL uv_wedge = ((LL) graph_[u].size() - 1) * ((LL) graph_[v].size() - 1) + 1;
            LL cn = 0;
            cn = fast_compute_common_bflys(u, v, two_hop_map);
#pragma omp critical
            {
                common_bflys_[std::make_pair(u, v)] = cn;
                common_bflys_[std::make_pair(v, u)] = cn;
            }
            long double base_eps = (long double) (cn + 1) / (long double) (uv_wedge);
            float tmp_eps = base_eps;
#pragma omp critical
            {
                similarity_square_[std::make_pair(u, v)] = tmp_eps;
                similarity_square_[std::make_pair(v, u)] = tmp_eps;
            };
        }
    }
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

void Graph::index_cluster_construct(bool use_hash) {
    index_core_cnt_left.resize(max_degree_ + 1, 0);
    index_core_cnt_right.resize(max_degree_ + 1, 0);
    for (int u = 0; u < node_num; ++u) {
        if (u % (node_num / 100 + 1) == 0 && node_num > 1000) std::cerr << "current :: tag -> :: " << u << std::endl;
        // compute similarity of all edges connected to u
        std::unordered_map<int, int> two_hop_map;
        if (use_hash) {
            get_two_hop_map(u, two_hop_map);
        }
        for (auto v: graph_[u]) {
            if (v < u) continue;
            LL uv_wedge = ((LL) graph_[u].size() - 1) * ((LL) graph_[v].size() - 1) + 1;
            LL cn = 0;
            if (use_hash)
                cn = fast_compute_common_bflys(u, v, two_hop_map);
            else
                cn = naive_compute_common_bflys(u, v);
            common_bflys_[std::make_pair(u, v)] = cn;
            common_bflys_[std::make_pair(v, u)] = cn;
            long double base_eps = (long double) (cn + 1) / (long double) (uv_wedge);
            float tmp_eps = base_eps;
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

void Graph::index_parallel_cluster_construct(int threads) {
    index_core_cnt_left.resize(max_degree_ + 1, 0);
    index_core_cnt_right.resize(max_degree_ + 1, 0);
    unsigned check_tag = 0;
#pragma omp parallel for num_threads(threads)
    for (int u = 0; u < node_num; ++u) {
#pragma omp critical
        {
            check_tag++;
            if (check_tag % (node_num / 100 + 1) == 0 && node_num > 1000)
                std::cerr << "current :: " << check_tag << std::endl;
        }
        std::unordered_map<int, int> two_hop_map;
        get_two_hop_map(u, two_hop_map);
        for (auto v: graph_[u]) {
            if (v < u) continue;
            LL uv_wedge = ((LL) graph_[u].size() - 1) * ((LL) graph_[v].size() - 1) + 1;
            LL cn = 0;
            cn = fast_compute_common_bflys(u, v, two_hop_map);
#pragma omp critical
            {
                common_bflys_[std::make_pair(u, v)] = cn;
                common_bflys_[std::make_pair(v, u)] = cn;
            }
            long double base_eps = (long double) (cn + 1) / (long double) (uv_wedge);
            float tmp_eps = base_eps;
#pragma omp critical
            {
                similarity_square_[std::make_pair(u, v)] = tmp_eps;
                similarity_square_[std::make_pair(v, u)] = tmp_eps;
            }
        }
#pragma omp critical
        {
            if (u <= left_nodes)
                ++index_core_cnt_left[graph_[u].size()];
            else
                ++index_core_cnt_right[graph_[u].size()];
            sort_nbr_by_similarity(u);
        }
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


void Graph::naive_reconstruct_cluster_construct(bool use_hash) {
    boost::unordered_map<std::pair<int, int>, bool, boost::hash<std::pair<int, int>>> check_tag;
    std::vector<std::vector<int> > two_hop_graph_;
    two_hop_graph_.resize(node_num);
    for (int i = 0; i < graph_.size(); i++) {
        for (auto neighbor: graph_[i]) {
            check_tag[std::make_pair(i, neighbor)] = true;
            check_tag[std::make_pair(neighbor, i)] = true;
            for (auto two_hop: graph_[neighbor]) {
                if (two_hop == i) continue;
                two_hop_graph_[i].push_back(two_hop);
            }
        }
    }
    std::cerr << "finished two hop" << std::endl;
    for (int i = 0; i < graph_.size(); i++) {
        for (auto two_hop: two_hop_graph_[i]) {
            if (!check_tag[std::make_pair(i, two_hop)]) {
                graph_[i].push_back(two_hop);
                graph_[two_hop].push_back(i);
                check_tag[std::make_pair(i, two_hop)] = true;
                check_tag[std::make_pair(two_hop, i)] = true;
            }
        }
    }


    for (int u = 0; u < node_num; u++) {
        if (u % (node_num / 100 + 1) == 0) std::cerr << "current :: tag -> :: " << u << std::endl;
        for (auto v: graph_[u]) {
            if (v < u) continue;
            LL cn = exact_neighbor_intersections(u, v) + 2;
            LL u_deg = (LL) graph_[u].size() + 1;
            LL v_deg = (LL) graph_[v].size() + 1;
            common_bflys_[std::make_pair(u, v)] = cn;
            common_bflys_[std::make_pair(v, u)] = cn;
            long double base_eps = (long double) (cn * cn) / (long double) (u_deg * v_deg);
            float tmp_eps = base_eps;
            similarity_square_[std::make_pair(u, v)] = tmp_eps;
            similarity_square_[std::make_pair(v, u)] = tmp_eps;
        }
    }
}
