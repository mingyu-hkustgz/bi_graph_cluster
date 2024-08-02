#include <iostream>
#include <fstream>
#include <ctime>
#include <cmath>
#include <getopt.h>
#include "graph.h"

#define HASHMAP
using namespace std;

int main(int argc, char *argv[]) {
    const struct option longopts[] = {
            // General Parameter
            {"help",     no_argument,       0, 'h'},

            // graph Path
            {"graph",    required_argument, 0, 'g'},
            // cluster parameters
            {"miuleft",  required_argument, 0, 'l'},
            {"miuright", required_argument, 0, 'r'},
            {"eps",      required_argument, 0, 'e'},
            {"index",    required_argument, 0, 'i'},
            // method type
            {"method",   required_argument, 0, 'm'},

            // result save path
            {"save",     required_argument, 0, 's'}
    };

    int ind;
    int iarg = 0;
    opterr = 1;    //getopt error message (off: 0)

    char index_path[256] = "";
    char graph_path[256] = "";
    char logger_path[256] = "";
    char result_path[256] = "";
    char modularity_path[256] = "";
    while (iarg != -1) {
        iarg = getopt_long(argc, argv, "g:d:r:m:", longopts, &ind);
        switch (iarg) {
            case 'g':
                if (optarg) strcpy(graph_path, optarg);
                break;
            case 'd':
                if (optarg)strcpy(logger_path, optarg);
                break;
            case 'r':
                if (optarg)strcpy(result_path, optarg);
                break;
            case 'm':
                if (optarg)strcpy(modularity_path, optarg);
                break;
        }
    }
    auto *graph = new Graph(graph_path);
    graph->load_graph();
    graph->naive_degree_cluster_construct();
    std::cerr << "index construct" << std::endl;
    std::unordered_map<int, int> result_mp;
    double max_origin_modularity = 0.0, max_split_modularity = 0.0;
    float origin_eps, split_eps;
    int origin_l, split_l;
    int split_hub_out_bound = graph->node_num / 5;
    auto tmp_graph = graph->graph_;
    for (int l = 1; l <= 10; l++) {
        for (float eps = 0.05; eps <= 1.0; eps += 0.05) {
            graph->degree_query_union(eps, l);
            result_mp.clear();
            graph->core_bm_.clear();
            int hub_out_count = 0;
            for (int i = 0; i < graph->node_num; i++) {
                if (graph->graph_[i].empty()) continue;
                int result = graph->fa_[i];
                if (result == i && !graph->core_bm_[i]) result = -1;
                result_mp[i] = result;
                if (result == -1) hub_out_count++;
            }
            std::cerr << l << " " << eps << " " << hub_out_count << std::endl;
            double origin_modularity = compute_modularity(tmp_graph, result_mp, "origin");
            double split_modularity = compute_modularity(tmp_graph, result_mp, "split");
            std::cerr << "Origin Modularity:: " << origin_modularity << std::endl;
            std::cerr << "Split Modularity:: " << split_modularity << std::endl;
            if (origin_modularity > max_origin_modularity) {
                max_origin_modularity = origin_modularity;
                origin_eps = eps;
                origin_l = l;
            }
            if (split_modularity > max_split_modularity && hub_out_count < split_hub_out_bound) {
                max_split_modularity = split_modularity;
                split_eps = eps;
                split_l = l;
            }
        }
    }
    std::cerr << "MAX Origin Modularity:: " << max_origin_modularity << " " << origin_eps << " " << origin_l
              << std::endl;
    std::cerr << "MAX Split Modularity:: " << max_split_modularity << " " << split_eps << " " << split_l << std::endl;


    graph->reconstruct_query_union(origin_eps, origin_l);
    result_mp.clear();
    graph->core_bm_.clear();
    int hub_out_count = 0;
    std::ofstream fout(result_path);
    fout << "id,tag" << std::endl;
    for (int i = 0; i < graph->node_num; i++) {
        if (graph->graph_[i].empty()) continue;
        int result = graph->fa_[i];
        if (result == i && !graph->core_bm_[i]) result = -1;
        result_mp[i] = result;
        if (result == -1) hub_out_count++;
        fout << i << "," << result << std::endl;
    }
    double origin_modularity = compute_modularity(graph->graph_, result_mp, "origin");
    double split_modularity = compute_modularity(graph->graph_, result_mp, "split");
    std::cerr << "Origin Modularity:: " << origin_modularity << std::endl;
    std::cerr << "Split Modularity:: " << split_modularity << std::endl;

    ofstream out(logger_path, std::ios::binary);
    unsigned dim = 2;
    for (auto u: result_mp) {
        out.write((char *) &dim, sizeof(unsigned));
        out.write((char *) &u.first, sizeof(unsigned));
        out.write((char *) &u.second, sizeof(unsigned));
    }

    ofstream mout(modularity_path);
    mout << "MAX Origin Modularity:: " << max_origin_modularity << " " << origin_eps << " " << origin_l
         << std::endl;
    mout << "MAX Split Modularity:: " << max_split_modularity << " " << split_eps << " " << split_l << std::endl;
    mout << "Origin Modularity:: " << origin_modularity << std::endl;
    mout << "Split Modularity:: " << split_modularity << std::endl;

    return 0;
}
/*
 */