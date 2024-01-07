//
// Created by Mingyu on 23-12-25.
//

#ifndef BI_GRAPH_CLUSTER_GRAPH_H
#define BI_GRAPH_CLUSTER_GRAPH_H
#define HASHMAP

#include "utils.h"

class Graph {

public:

    Graph(char *filename) {
        node_num = 0;
        edge_num = 0;
        fa_ = nullptr;
        graph_path = filename;
    }

    ~Graph() {
        std::vector<std::vector<int>>().swap(graph_);
        boost::unordered_map<std::pair<int, int>, int, boost::hash<std::pair<int, int>>>().swap(common_bflys_);
        boost::unordered_map<std::pair<int, int>, float, boost::hash<std::pair<int, int>>>().swap(similarity_square_);
        boost::dynamic_bitset<>().swap(core_bm_);
        std::vector<std::pair<int, int> >().swap(result_non_core_);
        std::vector<int>().swap(index_core_cnt_right);
        std::vector<int>().swap(index_core_cnt_left);
        std::vector<std::vector<int>>().swap(index_core_right);
        std::vector<std::vector<int>>().swap(index_core_left);
    }

    void load_graph() {
        std::ifstream fin(graph_path);
        int x, y;
        fin >> left_nodes >> right_nodes >> node_num >> edge_num;
        node_num += 2;
        graph_.resize(node_num + 2);
        max_degree_ = 0;
        boost::unordered_map<std::pair<int, int>, bool, boost::hash<std::pair<int, int>>> check_tag;
        for (int i = 0; i < edge_num; i++) {
            fin >> x >> y;
            if (!check_tag[std::make_pair(x, y + left_nodes + 1)]) {
                graph_[x].push_back(y + left_nodes + 1);
                check_tag[std::make_pair(x, y + left_nodes + 1)] = true;
            }
            if (!check_tag[std::make_pair(y + left_nodes + 1, x)]) {
                graph_[y + left_nodes + 1].push_back(x);
                check_tag[std::make_pair(y + left_nodes + 1, x)] = true;
            }
        }
        for (int i = 0; i < node_num; i++) max_degree_ = std::max(max_degree_, (int) graph_[i].size());
        std::cerr << "load graph finished with node num:: " << node_num << " " << "edge num::" << edge_num << std::endl;
        node_two_hop_.resize(node_num, -1);
    }

    void save_naive_data(char *filename) {
        std::ofstream fout(filename, std::ios::binary);
        fout.write((char *) &node_num, sizeof(int));
        fout.write((char *) &edge_num, sizeof(int));
        fout.write((char *) &left_nodes, sizeof(int));
        fout.write((char *) &right_nodes, sizeof(int));
        fout.write((char *) &max_degree_, sizeof(int));
        for (int i = 0; i < node_num; i++) {
            unsigned size = graph_[i].size();
            fout.write((char *) &size, sizeof(unsigned));
            fout.write((char *) graph_[i].data(), sizeof(int) * graph_[i].size());
        }
        for (int i = 0; i < node_num; i++) {
            for (auto v: graph_[i]) {
                float similarity = similarity_square_[std::make_pair(i, v)];
                fout.write((char *) &similarity, sizeof(float));
            }
        }
    }

    void load_naive_data(char *filename) {
        std::ifstream fin(filename, std::ios::binary);
        fin.read((char *) &node_num, sizeof(int));
        fin.read((char *) &edge_num, sizeof(int));
        fin.read((char *) &left_nodes, sizeof(int));
        fin.read((char *) &right_nodes, sizeof(int));
        fin.read((char *) &max_degree_, sizeof(int));
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
            }
        }
    }

    void save_index_data(char *filename) {
        std::ofstream fout(filename, std::ios::binary);
        fout.write((char *) &node_num, sizeof(int));
        fout.write((char *) &edge_num, sizeof(int));
        fout.write((char *) &left_nodes, sizeof(int));
        fout.write((char *) &right_nodes, sizeof(int));
        fout.write((char *) &max_degree_, sizeof(int));
        for (int i = 0; i < node_num; i++) {
            unsigned size = graph_[i].size();
            fout.write((char *) &size, sizeof(unsigned));
            fout.write((char *) graph_[i].data(), sizeof(int) * size);
        }
        for (int i = 0; i < node_num; i++) {
            for (auto v: graph_[i]) {
                float similarity = similarity_square_[std::make_pair(i, v)];
                fout.write((char *) &similarity, sizeof(float));
            }
        }
        for (int i = 0; i < node_num; i++) {
            for (auto v: graph_[i]) {
                unsigned com_fly = common_bflys_[std::make_pair(i, v)];
                fout.write((char *) &com_fly, sizeof(int));
            }
        }
        fout.write((char *) index_core_cnt_left.data(), sizeof(int) * (max_degree_ + 1));
        fout.write((char *) index_core_cnt_right.data(), sizeof(int) * (max_degree_ + 1));
        for (int i = 1; i <= max_degree_; i++) {
            fout.write((char *) index_core_left[i].data(), sizeof(int) * index_core_cnt_left[i]);
            fout.write((char *) index_core_right[i].data(), sizeof(int) * index_core_cnt_right[i]);
        }
        fout.write((char *) node_two_hop_.data(), sizeof(LL) * node_num);
    }

    void load_index_data(char *filename) {
        std::ifstream fin(filename, std::ios::binary);
        fin.read((char *) &node_num, sizeof(int));
        fin.read((char *) &edge_num, sizeof(int));
        fin.read((char *) &left_nodes, sizeof(int));
        fin.read((char *) &right_nodes, sizeof(int));
        fin.read((char *) &max_degree_, sizeof(int));
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
            }
        }
        for (int i = 0; i < node_num; i++) {
            for (auto v: graph_[i]) {
                int com_fly;
                fin.read((char *) &com_fly, sizeof(int));
                common_bflys_[std::make_pair(i, v)] = com_fly;
                common_bflys_[std::make_pair(v, i)] = com_fly;
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
        }
        node_two_hop_.resize(node_num);
        fin.read((char *) node_two_hop_.data(), sizeof(LL) * node_num);
    }


    int find_root(int u) const {
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
        if (rv < ru)
            fa_[ru] = rv;
        else
            fa_[rv] = ru;
    }

    int exact_neighbor_intersections(int u, int v) {
        int common = 0;
        if (graph_[u].size() > graph_[v].size()) std::swap(u, v);
        std::unordered_set<int> set;
        for (auto neighbor: graph_[u]) set.insert(neighbor);
        for (auto neighbor: graph_[v]) {
            if (set.find(neighbor) != set.end())
                common++;
        }
        return common;
    }

    int naive_compute_common_bflys(int u, int v) {
        int bfc_per_edge = 0;
        for (auto neighbor: graph_[u]) {
            if (neighbor != v) {
                bfc_per_edge += exact_neighbor_intersections(neighbor, v) - 1;
            }
        }
        return bfc_per_edge;
    }

    int fast_compute_common_bflys(int u, int v, std::unordered_map<int, int> &index_map) {
        int bfc_per_edge = 1;
        for (auto neighbor: graph_[v]) {
            bfc_per_edge += index_map[neighbor];
        }
        bfc_per_edge -= (int) graph_[v].size();
        bfc_per_edge -= (int) graph_[u].size();
        return bfc_per_edge;
    }

    void get_two_hop_map(int u, std::unordered_map<int, int> &index_map) {
        if (node_two_hop_[u] == -1) {
            node_two_hop_[u] = 0;
            for (auto neighbor: graph_[u]) {
                for (auto two_hop_neighborhood: graph_[neighbor]) {
                    index_map[two_hop_neighborhood]++;
                    if (two_hop_neighborhood != u) node_two_hop_[u]++;
                }
            }
        } else {
            for (auto neighbor: graph_[u]) {
                for (auto two_hop_neighborhood: graph_[neighbor]) {
                    index_map[two_hop_neighborhood]++;
                }
            }
        }
    }

    void get_two_hop_count(int u) {
        node_two_hop_[u] = 0;
        for (auto neighbor: graph_[u]) {
            for (auto two_hop_neighborhood: graph_[neighbor]) {
                if (two_hop_neighborhood != u) node_two_hop_[u]++;
            }
        }
    }


    void naive_cluster_construct() {
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

    void degree_cluster_construct() {
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

    void sort_nbr_by_similarity(int u) {
        auto &tmp_square = similarity_square_;
        std::sort(graph_[u].begin(), graph_[u].end(), [&tmp_square, u](int a, int b) {
            return tmp_square[std::make_pair(u, a)] > tmp_square[std::make_pair(u, b)];
        });
    }


    void sort_cores(int eps_deg) {
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


    void index_cluster_construct() {
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


    void naive_query_two_stage(float eps, int l_miu, int r_miu) {
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

    void naive_query_union(float eps, int l_miu, int r_miu) {
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

    void index_query_union(float eps, int l_miu, int r_miu) {
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

    void index_query_two_stage(float eps, int l_miu, int r_miu) {
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


    void dynamic_index_init(char *filename) {
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
                unsigned id = index_core_left[i][j];
                cores_mp_right_[i].emplace(nbr_order_[id][i - 1], id);
            }
        }
    }

    void update_all_core_order_naive(int u) {
        int deg_u = (int) nbr_[u].size();
        int i = 0;
        for (auto it = nbr_mp_[u].begin(); it != nbr_mp_[u].end(); ++it, ++i) {
            if (it->second <= left_nodes) {
                auto range = cores_mp_left_[i + 1].equal_range(nbr_order_[u][i]);
                for (auto rit = range.first; rit != range.second; ++rit) {
                    if (rit->second == u) {
                        cores_mp_left_[i + 1].erase(rit);
                        break;
                    }
                }
                cores_mp_left_[i + 1].insert(std::make_pair(it->first, u));
            } else {
                auto range = cores_mp_right_[i + 1].equal_range(nbr_order_[u][i]);
                for (auto rit = range.first; rit != range.second; ++rit) {
                    if (rit->second == u) {
                        cores_mp_right_[i + 1].erase(rit);
                        break;
                    }
                }
                cores_mp_right_[i + 1].insert(std::make_pair(it->first, u));
            }
            nbr_order_[u][i] = it->first;
        }
    }

    void update_all_core_order(int u) {
        int deg_u = (int) nbr_[u].size();
        int i = 0;
        for (auto it: nbr_mp_[u]) {
            i++;
            if (it.first == nbr_order_[u][i]) continue;
            if (i > 0 && it.second <= left_nodes) {
                auto range = cores_mp_left_[i + 1].equal_range(nbr_order_[u][i]);
                for (auto rit = range.first; rit != range.second; ++rit) {
                    if (rit->second == u) {
                        cores_mp_left_[i + 1].erase(rit);
                        cores_mp_left_[i + 1].insert(std::make_pair(it.first, u));
                        break;
                    }
                }
            }
            if (i > 0 && it.second > left_nodes) {
                auto range = cores_mp_right_[i + 1].equal_range(nbr_order_[u][i]);
                for (auto rit = range.first; rit != range.second; ++rit) {
                    if (rit->second == u) {
                        cores_mp_right_[i + 1].erase(rit);
                        cores_mp_right_[i + 1].insert(std::make_pair(it.first, u));
                        break;
                    }
                }
            }
            nbr_order_[u][i] = it.first;
        }
    }

    void update_nbr_order(int u, int root, float new_ss, float old_ss) {

        auto range = nbr_mp_[u].equal_range(old_ss);

        auto n_it = range.first;
        for (; n_it != range.second; ++n_it) {
            if (n_it->second == root) {

                nbr_mp_[u].erase(n_it);
                break;
            }
        }
        nbr_mp_[u].insert(std::make_pair(new_ss, root));
    }

    void remove_highest_core(int u) {
        int deg = (int) nbr_[u].size();
        float min_ss = nbr_mp_[u].rbegin()->first;
        if (u <= left_nodes) {
            auto range = cores_mp_left_[deg].equal_range(min_ss);
            for (auto it = range.first; it != range.second; ++it) {
                if (it->second == u) {
                    cores_mp_left_[deg].erase(it);
                    break;
                }
            }
        } else {
            auto range = cores_mp_right_[deg].equal_range(min_ss);
            for (auto it = range.first; it != range.second; ++it) {
                if (it->second == u) {
                    cores_mp_right_[deg].erase(it);
                    break;
                }
            }
        }
        if (!nbr_order_[u].empty()) nbr_order_[u].pop_back();
    }

    void update_core_order(int u, float new_ss, float old_ss) {
        int deg_u = (int) nbr_order_[u].size();
        bool enter_range = false;
        float pre = new_ss;
        float old;
        if (new_ss > old_ss) {

            for (int i = 0; i < deg_u; ++i) {
                if (nbr_order_[u][i] >= new_ss) continue;

                old = nbr_order_[u][i];
                std::swap(nbr_order_[u][i], pre);
                if (u <= left_nodes) {
                    auto range = cores_mp_left_[i + 1].equal_range(old);
                    for (auto it = range.first; it != range.second; ++it) {
                        if (it->second == u) {
                            cores_mp_left_[i + 1].erase(it);
                            cores_mp_left_[i + 1].emplace(nbr_order_[u][i], u);
                            break;
                        }
                    }
                } else {
                    auto range = cores_mp_right_[i + 1].equal_range(old);
                    for (auto it = range.first; it != range.second; ++it) {
                        if (it->second == u) {
                            cores_mp_right_[i + 1].erase(it);
                            cores_mp_right_[i + 1].emplace(nbr_order_[u][i], u);
                            break;
                        }
                    }
                }
                if (old <= old_ss) break;
            }
        } else {
            for (int i = deg_u - 1; i >= 0; --i) {
                if (nbr_order_[u][i] <= new_ss) continue;

                old = nbr_order_[u][i];
                // update
                std::swap(nbr_order_[u][i], pre);
                if (u <= left_nodes) {
                    auto range = cores_mp_left_[i + 1].equal_range(old);
                    for (auto it = range.first; it != range.second; ++it) {
                        if (it->second == u) {
                            cores_mp_left_[i + 1].erase(it);
                            cores_mp_left_[i + 1].emplace(nbr_order_[u][i], u);
                            break;
                        }
                    }
                } else {
                    auto range = cores_mp_right_[i + 1].equal_range(old);
                    for (auto it = range.first; it != range.second; ++it) {
                        if (it->second == u) {
                            cores_mp_right_[i + 1].erase(it);
                            cores_mp_right_[i + 1].emplace(nbr_order_[u][i], u);
                            break;
                        }
                    }
                }
                if (old >= old_ss) break;
            }
        }

    }


    void dyc_update_nbr(int a, int old_deg, int skip_id) {

        // mark each a's two hop neighbor via unordered map
        std::unordered_map<int, int> two_hop_map;
        get_two_hop_map(a, two_hop_map);
        int deg_a = (int) nbr_[a].size() - 1;

        for (auto a_it = nbr_[a].begin(); a_it != nbr_[a].end(); ++a_it) {

            int n_id = a_it->first;
            if (skip_id != -1 && n_id == skip_id) continue;
            int n_deg = (int) nbr_[n_id].size() - 1;

            int old_cn = a_it->second;
            int cn = fast_compute_common_bflys(a, n_id, two_hop_map);


            float new_ss = (float) cn * (float) cn / (n_deg) * (deg_a);

            // erase old similarity square from n_id's map
            if (old_cn != -1) {
                float old_ss = (float) old_cn * (float) old_cn / ((n_deg) * (old_deg));

                auto range = nbr_mp_[n_id].equal_range(old_ss);
                auto r_it = range.first;
                for (; r_it != range.second; ++r_it) {
                    if (r_it->second == a) {
                        nbr_mp_[n_id].erase(r_it);
                        break;
                    }
                }
            }

            a_it->second = cn;
            nbr_[n_id][a] = cn;

            nbr_mp_[n_id].insert(std::make_pair(new_ss, a));
            nbr_mp_[a].insert(std::make_pair(new_ss, n_id));
            update_all_core_order_naive(n_id);
        }
        // recover bitmap
        for (auto &a_it: nbr_[a]) {
            int n_id = a_it.first;
            dynamic_bm_[n_id] = false;
        }
        update_all_core_order_naive(a);
    }


    void statistics_eps_per_edge(char *filename) {
        std::vector<float> stat_res;
        for (int i = 0; i < node_num; i++) {
            for (auto v: graph_[i]) {
                stat_res.push_back(similarity_square_[std::make_pair(i, v)]);
            }
        }
        std::sort(stat_res.begin(), stat_res.end());
        int quantile = 0;
        for (int i = (stat_res.size() / 10); i < stat_res.size(); i += (stat_res.size() / 10)) {
            quantile++;
            std::cerr << quantile << "/10:: " << stat_res[i] << std::endl;
        }
        std::cerr << "max :: " << stat_res[stat_res.size() - 1] << std::endl;
        std::ofstream fout(filename, std::ios::binary);
        unsigned num = stat_res.size();
        fout.write((char *) &num, sizeof(unsigned));
        fout.write((char *) stat_res.data(), sizeof(float) * num);
    }

    void generate_test_examples(int n) {
        node_num = n;
        edge_num = n;
        int cnt = edge_num;
        left_nodes = node_num / 2;
        right_nodes = node_num - left_nodes;
        bool vis[1000][1000];
        memset(vis, 0, sizeof(vis));
        std::cerr << node_num << " " << edge_num << std::endl;
        graph_.resize(node_num);
        srand(123);
        while (cnt--) {
            while (true) {
                int u = rand() % left_nodes;
                int v = rand() % right_nodes + left_nodes;
                if (!vis[u][v]) {
                    graph_[u].push_back(v);
                    graph_[v].push_back(u);
                    std::cerr << u << " " << v << std::endl;
                    vis[u][v] = vis[v][u] = true;
                    break;
                }
            }
        }
        node_two_hop_.resize(node_num, -1);
        for (int i = 0; i < node_num; i++) max_degree_ = std::max(max_degree_, (int) graph_[i].size());
    }

    void generate_full_connect(int n) {
        node_num = n;
        edge_num = 0;
        int cnt = edge_num;
        left_nodes = node_num / 2;
        right_nodes = node_num - left_nodes;
        graph_.resize(node_num);
        for (int i = 0; i < left_nodes; i++) {
            for (int j = left_nodes; j < node_num; j++) {
                graph_[i].push_back(j);
                graph_[j].push_back(i);
                std::cerr << i << " " << j << std::endl;
                edge_num++;
            }
        }
        for (int i = 0; i < node_num; i++) max_degree_ = std::max(max_degree_, (int) graph_[i].size());
        node_two_hop_.resize(node_num, -1);
    }


    boost::unordered_map<std::pair<int, int>, int, boost::hash<std::pair<int, int>>> common_bflys_;
    boost::unordered_map<std::pair<int, int>, float, boost::hash<std::pair<int, int>>> similarity_square_;
    int node_num, edge_num;
    std::vector<std::vector<int>> graph_;
    std::vector<int> index_core_cnt_left, index_core_cnt_right;
    std::vector<std::vector<int>> index_core_left, index_core_right;
    /*
     * join set
     */
    int *fa_;

    float q_eps_;
    int q_miu_;
    int q_miu_left, q_miu_right;
    int max_degree_;
    int left_nodes, right_nodes;

    char *graph_path;
    boost::dynamic_bitset<> core_bm_;
    std::vector<std::pair<int, int>> result_non_core_;
    std::vector<LL> node_two_hop_;
    std::vector<std::unordered_map<int, bool> > hash_set;

    boost::dynamic_bitset<> dynamic_bm_;
    std::vector<std::multimap<float, int, std::greater<>>> cores_mp_left_, cores_mp_right_;
    std::vector<std::multimap<float, int, std::greater<>>> nbr_mp_;

    std::vector<float> *nbr_order_;
    std::unordered_map<int, int> *nbr_;


};


#endif //BI_GRAPH_CLUSTER_GRAPH_H
