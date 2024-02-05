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
        boost::unordered_map<std::pair<int, int>, LL, boost::hash<std::pair<int, int>>>().swap(common_bflys_);
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

    LL exact_neighbor_intersections(int u, int v);

    LL naive_compute_common_bflys(int u, int v);

    LL fast_compute_common_bflys(int u, int v, std::unordered_map<int, int> &index_map);

    void get_two_hop_map(int u, std::unordered_map<int, int> &index_map);

    void naive_cluster_construct(bool use_hash=true);

    void naive_reconstruct_cluster_construct(bool use_hash=true);

    void naive_parallel_cluster_construct(int threads);

    void sort_nbr_by_similarity(int u);

    void sort_cores(int eps_deg);

    void index_cluster_construct(bool use_hash=true);

    void index_parallel_cluster_construct(int threads);

    void naive_query_union(float eps, int l_miu, int r_miu);

    void index_query_union(float eps, int l_miu, int r_miu);

    void reconstruct_query_union(float eps, int miu);

    void dynamic_index_init(char *filename);

    void naive_insert_edge(int u, int v);

    void naive_delete_edge(int u, int v);

    void recompute_edge_similarity(int u, int v);

    int get_ave_left_degree(){
        LL degree_count = 0;
        for(int i=0;i<left_nodes;i++){
            degree_count += graph_[i].size();
        }
        degree_count /= left_nodes;
        return (int) degree_count;
    }

    int get_ave_right_degree(){
        LL degree_count = 0;
        for(int i=left_nodes;i<node_num;i++){
            degree_count += graph_[i].size();
        }
        degree_count /= right_nodes;
        return (int) degree_count;
    }



    LL randomized_compute_common_bflys(int a, int b) {
        std::random_device rd_edge;
        std::mt19937_64 eng_ran_bfc_per_edge(rd_edge());
        if (graph_[a].size() <= 1 || graph_[b].size() <= 1) {
            return 0;
        }
        std::uniform_int_distribution<int> dis_a(0, (int)graph_[a].size() - 1);
        std::uniform_int_distribution<int> dis_b(0, (int)graph_[b].size() - 1);
        LL res_ran_bfc_per_edge = 0;
        for (int i = 0; i < N_FAST_EDGE_BFC_ITERATIONS; i++) {
            int x = graph_[a][dis_a(eng_ran_bfc_per_edge)];
            int y = graph_[b][dis_b(eng_ran_bfc_per_edge)];
            if (x != b && y != a && binary_search(graph_[x].begin(), graph_[x].end(), y)) {
                res_ran_bfc_per_edge += ((LL) graph_[a].size() * ((LL) graph_[b].size()));
            }
        }
        res_ran_bfc_per_edge /= N_FAST_EDGE_BFC_ITERATIONS;
        return res_ran_bfc_per_edge;
    }

    void statistics_eps_per_edge(char *filename) {
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

    void save_similarity_edge(char *filename){
        std::ofstream fout(filename);
        fout<<node_num<<" "<<edge_num<<std::endl;
        for(int i=0;i<node_num;i++){
            for(auto u:graph_[i]){
                if(i < u)
                fout<<i<<" "<<u<<" "<<similarity_square_[std::make_pair(i,u)]<<std::endl;
            }
        }
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
    }


    boost::unordered_map<std::pair<int, int>, LL, boost::hash<std::pair<int, int>>> common_bflys_;
    boost::unordered_map<std::pair<int, int>, float, boost::hash<std::pair<int, int>>> similarity_square_;
    int node_num, edge_num;
    std::vector<std::vector<int>> graph_;
    std::vector<int> index_core_cnt_left, index_core_cnt_right;
    std::vector<std::vector<int>> index_core_left, index_core_right;
    /*
     * join set
     */
    int *fa_, *rank_;

    float q_eps_;
    int q_miu_;
    int q_miu_left, q_miu_right;
    int max_degree_;
    int left_nodes, right_nodes;

    int N_FAST_EDGE_BFC_ITERATIONS = 100;

    char *graph_path;
    boost::dynamic_bitset<> core_bm_;
    std::vector<std::pair<int, int>> result_non_core_;
    std::vector<std::unordered_map<int, bool> > hash_set;

    boost::dynamic_bitset<> dynamic_bm_;
    std::vector<std::multimap<float, int, std::greater<>>> cores_mp_left_, cores_mp_right_;
    std::vector<std::multimap<float, int, std::greater<>>> nbr_mp_;

    std::vector<float> *nbr_order_;
    std::unordered_map<int, int> *nbr_;
    std::vector<float> stat_res;

};


#endif //BI_GRAPH_CLUSTER_GRAPH_H
