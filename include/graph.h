//
// Created by BLD on 23-12-25.
//

#ifndef BI_GRAPH_CLUSTER_GRAPH_H
#define BI_GRAPH_CLUSTER_GRAPH_H

#include "utils.h"

class Graph {

    int find_root(int u) {
        int x = u;
        while (fa_[x] != x) x = fa_[x];
        while (fa_[u] != x) {
            int tmp = fa_[u];
            fa_[u] = x;
            u = tmp;
        }
        return x;
    }

    void set_union(int u, int v) {
        int ru = find_root(u);
        int rv = find_root(v);
        if (ru == rv) return;
        if (rank_[ru] < rank_[rv]) fa_[ru] = rv;
        else if (rank_[ru] > rank_[rv]) fa_[rv] = ru;
        else {
            fa_[rv] = ru;
            ++rank_[ru];
        }
    }

    int exact_neighbor_intersections(int u, int v) {
        int common = 0;
        int sz_u = offset_[u + 1] - offset_[u] + 1;
        int sz_v = offset_[v + 1] - offset_[v] + 1;
        if (sz_u > sz_v) std::swap(u, v);
        std::unordered_set<int> set;
        for (int i = offset_[u]; i < offset_[u + 1]; ++i) set.insert(edges_[i]);
        for (int j = offset_[v]; j < offset_[v + 1]; j++) {
            if (set.find(edges_[j]) != set.end())
                common++;
        }
        return common;
    }

    int naive_compute_common_bflys(int u, int v) {
        int bfc_per_edge = 0;
        for (int i = offset_[u]; i < offset_[u + 1]; ++i) {
            int neighbor = edges_[i];
            if (neighbor != v) {
                bfc_per_edge += exact_neighbor_intersections(neighbor, v) - 1;
            }
        }
        return bfc_per_edge;
    }


    int fast_compute_common_bflys(int u, int v) {
        std::unordered_map<int, int> index_map;
        for (unsigned int i = offset_[u]; i < offset_[u + 1]; ++i) {
            int neighbor = edges_[i];
            if (neighbor != v) {
                for (int j = offset_[neighbor]; j < offset_[neighbor + 1]; j++) {
                    int two_hop_neighborhood = edges_[j];
                    if (two_hop_neighborhood != u) {
                        index_map[two_hop_neighborhood]++;
                    }
                }
            }
        }
        int bfc_per_edge = 0;
        for (unsigned int i = offset_[v]; i < offset_[v + 1]; ++i) {
            int neighbor = edges_[i];
            bfc_per_edge += index_map[neighbor];
        }
        return bfc_per_edge;
    }

    void naive_cluster_construct() {
        for (int u = 0; u < node_num; u++) {
            LL u_deg = (LL) offset_[u + 1] - offset_[u] + 1;
            for (unsigned int i = offset_[u]; i < offset_[u + 1]; ++i) {
                int v = edges_[i];
                if (v < u) continue;
                int cn = fast_compute_common_bflys(u, v);
                common_bflys_[i] = cn;
                common_bflys_[reverse_[i]] = cn;
                LL v_deg = offset_[v + 1] - offset_[v] + 1;
                similarity_square_[i] = (float) cn * (float) cn / (u_deg * v_deg);
                similarity_square_[reverse_[i]] = similarity_square_[i];
            }
        }
    }

    void degree_cluster_construct() {
        int *head = new int[max_degree_ + 1];
        for (int i = 0; i <= max_degree_; ++i) head[i] = -1;
        int *next = new int[node_num];
        for (int i = 0; i < node_num; ++i) {
            int degree = offset_[i + 1] - offset_[i];
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
            for (unsigned int i = offset_[u]; i < offset_[u + 1]; ++i) cn_bm_.set(edges_[i], true);
            long long u_deg = offset_[u + 1] - offset_[u] + 1;

            for (unsigned int i = offset_[u]; i < offset_[u + 1]; ++i) {
                int v = edges_[i];
                if (pru[v]) continue;

                int cn = fast_compute_common_bflys(u, v);
                common_bflys_[i] = cn;
                common_bflys_[reverse_[i]] = cn;

                long long v_deg = offset_[v + 1] - offset_[v] + 1;
                similarity_square_[i] = (float) cn * (float) cn / (u_deg * v_deg);
                similarity_square_[reverse_[i]] = similarity_square_[i];
            }

            for (unsigned int i = offset_[u]; i < offset_[u + 1]; ++i) cn_bm_.set(edges_[i], false);

            pru.set(u, true);
            u = next[u];
        }
        delete[] head;
        delete[] next;
    }

    void sort_nbr_by_similarity(int u) {
        int len = offset_[u + 1] - offset_[u];
        for (int i = 0; i < len; ++i) sort_buf_[i] = i;
        float *tmp_square = similarity_square_;
        unsigned temp_offset = offset_[u];
        std::sort(sort_buf_, sort_buf_ + len, [tmp_square, temp_offset](int a, int b) {
            return tmp_square[temp_offset + a] > tmp_square[temp_offset + b];
        });

        int tmp_cn;
        int tmp_edge;
        float tmp_sim;
        unsigned int tmp_reverse;

        int tmp_oft;
        for (int pos = 0; pos < len - 1; ++pos) {
            if (pos == sort_buf_[pos] || sort_buf_[pos] == -1) continue;
            int i = pos;
            tmp_cn = common_bflys_[offset_[u] + i];
            tmp_edge = edges_[offset_[u] + i];
            tmp_sim = similarity_square_[offset_[u] + i];
            tmp_reverse = reverse_[offset_[u] + i];
            tmp_oft = i;

            while (true) {
                int t = sort_buf_[i];
                if (sort_buf_[t] == -1) {
                    common_bflys_[offset_[u] + i] = tmp_cn;
                    edges_[offset_[u] + i] = tmp_edge;
                    similarity_square_[offset_[u] + i] = tmp_sim;
                    reverse_[offset_[u] + i] = tmp_reverse;
                    sort_buf_[i] = -1;
                    break;
                }
                common_bflys_[offset_[u] + i] = common_bflys_[offset_[u] + t];
                edges_[offset_[u] + i] = edges_[offset_[u] + t];
                similarity_square_[offset_[u] + i] = similarity_square_[offset_[u] + t];
                reverse_[offset_[u] + i] = reverse_[offset_[u] + t];

                sort_buf_[i] = -1;
                i = t;
            }

        }
        // repair reverse array
        for (unsigned int i = offset_[u]; i < offset_[u + 1]; ++i) reverse_[reverse_[i]] = i;
    }


    void sort_cores(int eps_deg) {
        int cur = 0;
        if (eps_deg == 1) {
            // for(int i = 0; i < n_; ++i) idx1_cores_[eps_deg][i] = i;
            for (int i = 0; i < node_num; ++i) {
                if (offset_[i + 1] - offset_[i] < eps_deg) continue;
                index_core_[eps_deg][cur++] = i;
            }
        } else {
            for (int i = 0; i < index_core_cnt[eps_deg - 1]; ++i) {
                int id = index_core_[eps_deg - 1][i];
                if (offset_[id + 1] - offset_[id] < eps_deg) continue;
                index_core_[eps_deg][cur++] = id;
            }
        }
        float *tmp_square = similarity_square_;
        int *tmp_offset = offset_;
        std::sort(index_core_[eps_deg], index_core_[eps_deg] + index_core_cnt[eps_deg],
                  [eps_deg, tmp_square, tmp_offset](int u, int v) {
                      return tmp_square[tmp_offset[u] + eps_deg - 1] > tmp_square[tmp_offset[v] + eps_deg - 1];
                  });

    }


    void index_cluster_construct() {
        sort_buf_ = new int[max_degree_];

        index_core_cnt = new int[max_degree_ + 1];
        memset(index_core_cnt, 0, sizeof(int) * (max_degree_ + 1));
        for (int u = 0; u < node_num; ++u) {
            LL u_deg = (LL) offset_[u + 1] - offset_[u] + 1;
            // compute similarity of all edges connected to u
            for (unsigned int i = offset_[u]; i < offset_[u + 1]; ++i) {
                int v = edges_[i];
                if (v < u) continue;
                int cn = fast_compute_common_bflys(u, v);
                common_bflys_[i] = cn;
                common_bflys_[reverse_[i]] = cn;
                long long v_deg = offset_[v + 1] - offset_[v] + 1;
                similarity_square_[i] = (float) cn * (float) cn / (u_deg * v_deg);
                similarity_square_[reverse_[i]] = similarity_square_[i];
            }

            ++index_core_cnt[offset_[u + 1] - offset_[u]];
            sort_nbr_by_similarity(u);
            // quick_sort_nbr_by_similarity(u, offset_[u], offset_[u+1]);
        }

        // compute the number of vertices under each miu
        for (int i = max_degree_ - 1; i > 0; --i) {
            index_core_cnt[i] += index_core_cnt[i + 1];
        }

        index_core_ = new int *[max_degree_ + 1];
        // construct the hierarchy
        for (int i = 1; i <= max_degree_; ++i) {
            index_core_[i] = new int[index_core_cnt[i]];
            sort_cores(i);
        }
    }


    void naive_query_two_stage(float eps, int miu) {
        q_eps_ = eps;
        q_miu_ = miu;
        result_non_core_.clear();

        float eps_d = eps;
        eps_d *= eps_d;
        --miu;

        if (fa_ == nullptr) fa_ = new int[node_num];
        if (rank_ == nullptr) rank_ = new int[node_num];
        memset(rank_, 0, sizeof(int) * node_num);
        for (int i = 0; i < node_num; i++) fa_[i] = i;
        core_bm_.resize(node_num);

        // cluster core vertices
        int cnt;
        for (int i = 0; i < node_num; ++i) {
            cnt = 0;
            for (unsigned int j = offset_[i]; j < offset_[i + 1]; ++j) {
                if (similarity_square_[j] >= eps_d) ++cnt;
            }
            if (cnt < miu) continue;

            core_bm_.set(i, true);

            for (unsigned int j = offset_[i]; j < offset_[i + 1]; ++j) {
                int id = edges_[j];
                if (id > i) continue;
                if (core_bm_[id] && similarity_square_[j] >= eps_d) set_union(i, id);
            }
        }
        // cluster noncore vertices
        for (int i = 0; i < node_num; ++i) {
            if (!core_bm_[i]) continue;
            for (unsigned int j = offset_[i]; j < offset_[i + 1]; ++j) {
                int id = edges_[j];
                if (similarity_square_[j] < eps_d) continue;
                if (core_bm_[id] && find_root(id) == find_root(i)) continue;
                result_non_core_.emplace_back(find_root(i), id);
            }
        }

    }

    void naive_query_union(float eps, int miu) {
        q_eps_ = eps;
        q_miu_ = miu;
        result_non_core_.clear();

        float eps_d = eps;
        eps_d *= eps_d;
        --miu;

        if (fa_ == nullptr) fa_ = new int[node_num];
        if (rank_ == nullptr) rank_ = new int[node_num];
        memset(rank_, 0, sizeof(int) * node_num);
        for (int i = 0; i < node_num; i++) fa_[i] = i;
        core_bm_.resize(node_num);

        // cluster core vertices
        int cnt;
        for (int i = 0; i < node_num; ++i) {
            cnt = 0;
            for (unsigned int j = offset_[i]; j < offset_[i + 1]; ++j) {
                if (similarity_square_[j] >= eps_d) ++cnt;
            }
            if (cnt < miu) continue;

            core_bm_.set(i, true);

            for (unsigned int j = offset_[i]; j < offset_[i + 1]; ++j) {
                if (similarity_square_[j] < eps_d) continue;
                int id = edges_[j];

                if (id < i && core_bm_[id] && similarity_square_[j] >= eps_d) {
                    set_union(i, id);
                } else {
                    result_non_core_.emplace_back(find_root(i), id);
                }
            }
        }
    }

    void index_query_union(float eps, int miu) {
        q_eps_ = eps;
        q_miu_ = miu;
        --miu;
        if (fa_ == nullptr) fa_ = new int[node_num];
        for (int i = 0; i < node_num; i++) fa_[i] = i;
        core_bm_.resize(node_num);
        float eps_d = eps;
        eps_d *= eps_d;
        int cur;
        for (int i = 0; i < index_core_cnt[miu]; ++i) {
            int id = index_core_[miu][i];

            if (core_bm_[id]) continue;

            if (eps_d > similarity_square_[offset_[id] + miu - 1]) break;
            cur = id;
            std::queue<int> q;
            q.push(id);
            core_bm_[id] = true;
            while (!q.empty()) {
                int q_id = q.front();
                q.pop();
                fa_[q_id] = cur;

                for (unsigned int j = offset_[q_id]; j < offset_[q_id + 1]; ++j) {
                    int nbr_id = edges_[j];
                    if (core_bm_[nbr_id]) continue;
                    if (similarity_square_[j] < eps_d) break;

                    if (offset_[nbr_id + 1] - offset_[nbr_id] >= miu &&
                        similarity_square_[offset_[nbr_id] + miu - 1] >= eps_d) {
                        core_bm_[nbr_id] = true;
                        q.push(nbr_id);
                    } else {
                        result_non_core_.emplace_back(cur, nbr_id);
                    }
                }
            }
            ++cur;
        }
    }

    void index_query_two_stage(float eps, int miu) {
        q_eps_ = eps;
        q_miu_ = miu;
        --miu;
        if (fa_ == nullptr) fa_ = new int[node_num];
        for (int i = 0; i < node_num; i++) fa_[i] = i;
        core_bm_.resize(node_num);
        float eps_d = eps;
        eps_d *= eps_d;
        int cur;
        for (int i = 0; i < index_core_cnt[miu]; ++i) {
            int id = index_core_[miu][i];

            if (core_bm_[id]) continue;

            if (eps_d > similarity_square_[offset_[id] + miu - 1]) break;
            cur = id;
            std::queue<int> q;
            q.push(id);
            core_bm_[id] = true;
            while (!q.empty()) {
                int q_id = q.front();
                q.pop();
                fa_[q_id] = cur;

                for (unsigned int j = offset_[q_id]; j < offset_[q_id + 1]; ++j) {
                    int nbr_id = edges_[j];
                    if (core_bm_[nbr_id]) continue;
                    if (similarity_square_[j] < eps_d) break;

                    if (offset_[nbr_id + 1] - offset_[nbr_id] >= miu &&
                        similarity_square_[offset_[nbr_id] + miu - 1] >= eps_d) {
                        core_bm_[nbr_id] = true;
                        q.push(nbr_id);
                    }
                }
            }
            ++cur;
        }
        // cluster non-core
        for(int i = 0; i < index_core_cnt[miu]; ++i){
            int id = index_core_[miu][i];
            if(eps_d > similarity_square_[offset_[id]+miu-1]) break;
            for(unsigned int j = offset_[id]; j < offset_[id+1]; ++j){
                int nbr_id = edges_[j];
                if(core_bm_[nbr_id]) continue;
                result_non_core_.emplace_back(fa_[id],nbr_id);
            }
        }

    }


    float *similarity_square_;
    int *common_bflys_;
    int node_num, edge_num, left_num, right_num;
    int *edges_; // adjacent list for each vertex
    unsigned int *reverse_; // position of reverse edge in array edges
    int *offset_;
    int *sort_buf_;
    int *index_core_cnt;
    int **index_core_;
    /*
     * join set
     */
    int *fa_;
    int *rank_;

    float q_eps_;
    int q_miu_;
    int max_degree_;

    boost::dynamic_bitset<> dynamic_bm_;
    boost::dynamic_bitset<> core_bm_;
    std::vector<std::pair<int, int>> result_non_core_;

    std::vector<std::unordered_map<int, bool> > hash_set;
};


#endif //BI_GRAPH_CLUSTER_GRAPH_H
