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

    void load_graph();

    void save_naive_data(char *filename);

    void load_naive_data(char *filename);

    void save_index_data(char *filename);

    void load_index_data(char *filename);

    int find_root(int u) const;

    void set_union(int u, int v) const;

    int exact_neighbor_intersections(int u, int v);

    int naive_compute_common_bflys(int u, int v);

    int fast_compute_common_bflys(int u, int v, std::unordered_map<int, int> &index_map);

    void get_two_hop_map(int u, std::unordered_map<int, int> &index_map);

    void get_two_hop_count(int u);

    void naive_cluster_construct();

    void degree_cluster_construct();

    void sort_nbr_by_similarity(int u);

    void sort_cores(int eps_deg);

    void index_cluster_construct();

    void naive_query_two_stage(float eps, int l_miu, int r_miu);

    void naive_query_union(float eps, int l_miu, int r_miu);

    void index_query_union(float eps, int l_miu, int r_miu);

    void index_query_two_stage(float eps, int l_miu, int r_miu);

    void dynamic_index_init(char *filename);

    void naive_insert_edge(int u, int v);

    void naive_delete_edge(int u, int v);


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
