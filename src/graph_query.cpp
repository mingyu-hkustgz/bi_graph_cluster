#include "graph.h"

void Graph::naive_query_union(float eps, int l_miu, int r_miu) {
    q_eps_ = eps;
    q_miu_left = l_miu;
    q_miu_right = r_miu;
    result_non_core_.clear();

    float eps_d = eps;

    if (fa_ == nullptr) fa_ = new int[node_num];
    if (rank_ == nullptr) rank_ = new int[node_num];
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
    std::vector<int> hit_map;
    hit_map.resize(node_num, 0);
    for (auto item: result_non_core_) {
        if (hit_map[item.second] == -1) continue;
        if (!core_bm_[item.second]) {
            if (hit_map[item.second] == 0) hit_map[item.second] = find_root(item.first);
            else if (hit_map[item.second] != find_root(item.first)) hit_map[item.second] = -1;
        }
    }
    for (auto item: result_non_core_) {
        if (hit_map[item.second] == -1) fa_[item.second] = -1;
        else
            fa_[item.second] = find_root(item.first);
    }
    for (int i = 0; i < node_num; i++) {
        if (fa_[i] != -1)
            fa_[i] = find_root(i);
        if (!core_bm_[i] && fa_[i] == i) fa_[i] = -1;
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
    int cur;
    for (int i = 0; i < index_core_cnt_left[l_miu]; ++i) {
        int id = index_core_left[l_miu][i];
//        std::cerr << id << " " << graph_[id][l_miu - 1] << " index " << core_bm_[id] << " "
//                  << similarity_square_[std::make_pair(id, graph_[id][l_miu - 1])] << std::endl;
        if (core_bm_[id]) continue;
        if (eps_d > similarity_square_[std::make_pair(id, graph_[id][l_miu - 1])]) break;
        cur = id;
        std::queue<int> q;
        q.push(id);
        core_bm_[id] = true;
        while (!q.empty()) {
            int q_id = q.front();
            q.pop();
            fa_[q_id] = cur;
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
                    if (fa_[nbr_id] == nbr_id || fa_[nbr_id] == cur) fa_[nbr_id] = cur;
                    else fa_[nbr_id] = -1;
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
            fa_[q_id] = cur;
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
                    if (fa_[nbr_id] == nbr_id || fa_[nbr_id] == cur) fa_[nbr_id] = cur;
                    else fa_[nbr_id] = -1;
                    result_non_core_.emplace_back(cur, nbr_id);
                }
            }
        }
    }
}

void Graph::reconstruct_query_union(float eps, int miu) {
    result_non_core_.clear();

    float eps_d = eps;

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
        if (cnt < miu) continue;

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
    std::vector<int> hit_map;
    hit_map.resize(node_num, 0);
    for (auto item: result_non_core_) {
        if (hit_map[item.second] == -1) continue;
        if (!core_bm_[item.second]) {
            if (hit_map[item.second] == 0) hit_map[item.second] = find_root(item.first);
            else if (hit_map[item.second] != find_root(item.first)) hit_map[item.second] = -1;
        }
    }
    for (auto item: result_non_core_) {
        if (hit_map[item.second] == -1) fa_[item.second] = -1;
        else
            fa_[item.second] = find_root(item.first);
    }
    for (int i = 0; i < node_num; i++) {
        if (fa_[i] != -1)
            fa_[i] = find_root(i);
        if (!core_bm_[i] && fa_[i] == i) fa_[i] = -1;
    }
}

void Graph::degree_query_union(float eps, int miu) {
    result_non_core_.clear();

    float eps_d = eps;

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
        if (cnt < miu) continue;

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
    std::vector<int> hit_map;
    hit_map.resize(node_num, 0);
    for (auto item: result_non_core_) {
        if (hit_map[item.second] == -1) continue;
        if (!core_bm_[item.second]) {
            if (hit_map[item.second] == 0) hit_map[item.second] = find_root(item.first);
            else if (hit_map[item.second] != find_root(item.first)) hit_map[item.second] = -1;
        }
    }
    for (auto item: result_non_core_) {
        if (hit_map[item.second] == -1) fa_[item.second] = -1;
        else
            fa_[item.second] = find_root(item.first);
    }
    for (int i = 0; i < node_num; i++) {
        if (fa_[i] != -1)
            fa_[i] = find_root(i);
        if (!core_bm_[i] && fa_[i] == i) fa_[i] = -1;
    }
}


void Graph::dynamic_query_union(float eps, int l_miu, int r_miu) {
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
    int cur;
    for (auto item = cores_mp_left_[l_miu].begin(); item != cores_mp_left_[l_miu].end(); ++item) {
        int id = item->second;
//        std::cerr << id << " " << graph_[id][l_miu - 1] << " left " << core_bm_[id] << " "
//                  << similarity_square_[std::make_pair(id, graph_[id][l_miu - 1])] << std::endl;
        if (core_bm_[id]) continue;
        if (eps_d > similarity_square_[std::make_pair(id, graph_[id][l_miu - 1])]) break;
        cur = id;
        std::queue<int> q;
        q.push(id);
        core_bm_[id] = true;
        while (!q.empty()) {
            int q_id = q.front();
            q.pop();
            fa_[q_id] = cur;
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
                    if (fa_[nbr_id] == nbr_id || fa_[nbr_id] == cur) fa_[nbr_id] = cur;
                    else fa_[nbr_id] = -1;
                    result_non_core_.emplace_back(cur, nbr_id);
                }
            }
        }
    }
    /*
     * right again
     */
    for (auto item: cores_mp_right_[r_miu]) {
        int id = item.second;
        if (core_bm_[id]) continue;
        if (eps_d > similarity_square_[std::make_pair(id, graph_[id][r_miu - 1])]) break;
        cur = id;
        std::queue<int> q;
        q.push(id);
        core_bm_[id] = true;
        while (!q.empty()) {
            int q_id = q.front();
            q.pop();
            fa_[q_id] = cur;
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
                    if (fa_[nbr_id] == nbr_id || fa_[nbr_id] == cur) fa_[nbr_id] = cur;
                    else fa_[nbr_id] = -1;
                    result_non_core_.emplace_back(cur, nbr_id);
                }
            }
        }
    }
}
