//
// Created by Mingyu on 24-1-7.
//

#include "graph.h"

void Graph::naive_query_two_stage(float eps, int l_miu, int r_miu) {
    q_eps_ = eps;
    q_miu_left = l_miu;
    q_miu_right = r_miu;
    result_non_core_.clear();

    float eps_d = eps;
    eps_d *= eps_d;

    if (fa_ == nullptr) fa_ = new int[node_num];
    for (int i = 0; i < node_num; i++) fa_[i] = i;
    core_bm_.resize(node_num);

    // cluster core vertices
    int cnt;
    for (int i = 0; i < node_num; ++i) {
        cnt = 0;
        for (auto neighbor: graph_[i]) {
            if (similarity_square_[std::make_pair(i, neighbor)] >= eps_d) ++cnt;
        }
        if (i <= left_nodes) {
            if (cnt < l_miu) continue;
        } else {
            if (cnt < r_miu) continue;
        }
        core_bm_.set(i, true);

        for (auto id: graph_[i]) {
            if (id > i) continue;
            if (core_bm_[id] && similarity_square_[std::make_pair(i, id)] >= eps_d) set_union(i, id);
        }
    }
    // cluster noncore vertices
    for (int i = 0; i < node_num; ++i) {
        if (!core_bm_[i]) continue;
        for (auto id: graph_[i]) {
            if (similarity_square_[std::make_pair(i, id)] < eps_d) continue;
            if (core_bm_[id] && find_root(id) == find_root(i)) continue;
            result_non_core_.emplace_back(find_root(i), id);
        }
    }

}

void Graph::naive_query_union(float eps, int l_miu, int r_miu) {
    q_eps_ = eps;
    q_miu_left = l_miu;
    q_miu_right = r_miu;
    result_non_core_.clear();

    float eps_d = eps;
    eps_d *= eps_d;

    if (fa_ == nullptr) fa_ = new int[node_num];
    for (int i = 0; i < node_num; i++) fa_[i] = i;
    core_bm_.resize(node_num);

    // cluster core vertices
    int cnt;
    for (int i = 0; i < node_num; ++i) {
        cnt = 0;
        for (auto neighbor: graph_[i]) {
            if (similarity_square_[std::make_pair(i, neighbor)] >= eps_d) ++cnt;
        }
        if (i <= left_nodes) {
            if (cnt < l_miu) continue;
        } else {
            if (cnt < r_miu) continue;
        }

        core_bm_[i] = true;

        for (auto id: graph_[i]) {
            if (similarity_square_[std::make_pair(i, id)] < eps_d) continue;

            if (id < i && core_bm_[id] && similarity_square_[std::make_pair(i, id)] >= eps_d) {
                set_union(i, id);
            } else {
                result_non_core_.emplace_back(find_root(i), id);
            }
        }
    }
}

void Graph::index_query_union(float eps, int l_miu, int r_miu) {
    if (l_miu == 0 || r_miu == 0) {
        std::cerr << "l miu and r mid must > 0";
        return;
    }
    q_eps_ = eps;
    q_miu_left = l_miu;
    q_miu_right = r_miu;
    if (fa_ == nullptr) fa_ = new int[node_num];
    for (int i = 0; i < node_num; i++) fa_[i] = i;
    core_bm_.resize(node_num);
    float eps_d = eps;
    eps_d *= eps_d;
    int cur;
    for (int i = 0; i < index_core_cnt_left[l_miu]; ++i) {
        int id = index_core_left[l_miu][i];
        if (core_bm_[id]) continue;
        if (eps_d > similarity_square_[std::make_pair(id, graph_[id][l_miu - 1])]) break;
        cur = id;
        std::queue<int> q;
        q.push(id);
        core_bm_[id] = true;
        while (!q.empty()) {
            int q_id = q.front();
            q.pop();
            set_union(cur, q_id);
            for (auto nbr_id: graph_[q_id]) {
                if (core_bm_[nbr_id]) continue;
                if (similarity_square_[std::make_pair(q_id, nbr_id)] < eps_d) break;
                int miu = l_miu;
                if (nbr_id > left_nodes) miu = r_miu;
                if (graph_[nbr_id].size() >= miu &&
                    similarity_square_[std::make_pair(nbr_id, graph_[nbr_id][miu - 1])] >= eps_d) {
                    core_bm_[nbr_id] = true;
                    q.push(nbr_id);
                } else {
                    result_non_core_.emplace_back(cur, nbr_id);
                }
            }
        }
    }
    /*
     * right again
     */
    for (int i = 0; i < index_core_cnt_right[r_miu]; ++i) {
        int id = index_core_right[r_miu][i];
        if (core_bm_[id]) continue;
        if (eps_d > similarity_square_[std::make_pair(id, graph_[id][r_miu - 1])]) break;
        cur = id;
        std::queue<int> q;
        q.push(id);
        core_bm_[id] = true;
        while (!q.empty()) {
            int q_id = q.front();
            q.pop();
            set_union(cur, q_id);
            for (auto nbr_id: graph_[q_id]) {
                if (core_bm_[nbr_id]) continue;
                if (similarity_square_[std::make_pair(q_id, nbr_id)] < eps_d) break;
                int miu = l_miu;
                if (nbr_id > left_nodes) miu = r_miu;
                if (graph_[nbr_id].size() >= miu &&
                    similarity_square_[std::make_pair(nbr_id, graph_[nbr_id][miu - 1])] >= eps_d) {
                    core_bm_[nbr_id] = true;
                    q.push(nbr_id);
                } else {
                    result_non_core_.emplace_back(cur, nbr_id);
                }
            }
        }
    }
}

void Graph::index_query_two_stage(float eps, int l_miu, int r_miu) {
    q_eps_ = eps;
    q_miu_left = l_miu;
    q_miu_right = r_miu;
    if (fa_ == nullptr) fa_ = new int[node_num];
    for (int i = 0; i < node_num; i++) fa_[i] = i;
    core_bm_.resize(node_num);
    float eps_d = eps;
    eps_d *= eps_d;
    int cur;
    for (int i = 0; i < index_core_cnt_left[l_miu]; ++i) {
        int id = index_core_left[l_miu][i];
        if (core_bm_[id]) continue;
        if (eps_d > similarity_square_[std::make_pair(id, graph_[id][l_miu - 1])]) break;
        cur = id;
        std::queue<int> q;
        q.push(id);
        core_bm_[id] = true;
        while (!q.empty()) {
            int q_id = q.front();
            q.pop();
            set_union(cur, q_id);
            for (auto nbr_id: graph_[q_id]) {
                if (core_bm_[nbr_id]) continue;
                if (similarity_square_[std::make_pair(q_id, nbr_id)] < eps_d) break;
                int miu = l_miu;
                if (nbr_id > left_nodes) miu = r_miu;
                if (graph_[nbr_id].size() >= miu &&
                    similarity_square_[std::make_pair(nbr_id, graph_[nbr_id][miu - 1])] >= eps_d) {
                    core_bm_[nbr_id] = true;
                    q.push(nbr_id);
                }
            }
        }
    }
    // cluster non-core
    for (int i = 0; i < index_core_cnt_left[l_miu]; ++i) {
        int id = index_core_left[l_miu][i];
        if (eps_d > similarity_square_[std::make_pair(id, graph_[id][l_miu - 1])]) break;
        for (auto nbr_id: graph_[id]) {
            if (core_bm_[nbr_id]) continue;
            result_non_core_.emplace_back(fa_[id], nbr_id);
        }
    }
    /*
     * right again
     */
    for (int i = 0; i < index_core_cnt_right[r_miu]; ++i) {
        int id = index_core_right[r_miu][i];
        if (core_bm_[id]) continue;
        if (eps_d > similarity_square_[std::make_pair(id, graph_[id][r_miu - 1])]) break;
        cur = id;
        std::queue<int> q;
        q.push(id);
        core_bm_[id] = true;
        while (!q.empty()) {
            int q_id = q.front();
            q.pop();
            set_union(cur, q_id);
            for (auto nbr_id: graph_[q_id]) {
                if (core_bm_[nbr_id]) continue;
                if (similarity_square_[std::make_pair(q_id, nbr_id)] < eps_d) break;
                int miu = l_miu;
                if (nbr_id > left_nodes) miu = r_miu;
                if (graph_[nbr_id].size() >= miu &&
                    similarity_square_[std::make_pair(nbr_id, graph_[nbr_id][miu - 1])] >= eps_d) {
                    core_bm_[nbr_id] = true;
                    q.push(nbr_id);
                }
            }
        }
        ++cur;
    }
    // cluster non-core
    for (int i = 0; i < index_core_cnt_right[r_miu]; ++i) {
        int id = index_core_right[r_miu][i];
        if (eps_d > similarity_square_[std::make_pair(id, graph_[id][r_miu - 1])]) break;
        for (auto nbr_id: graph_[id]) {
            if (core_bm_[nbr_id]) continue;
            result_non_core_.emplace_back(fa_[id], nbr_id);
        }
    }
}