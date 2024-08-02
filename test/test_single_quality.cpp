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

    char graph_path[256] = "";
    char modularity_path[256] = "";
    int l, r;
    float eps = 0.1;

    while (iarg != -1) {
        iarg = getopt_long(argc, argv, "g:l:r:e:m:", longopts, &ind);
        switch (iarg) {
            case 'g':
                if (optarg) strcpy(graph_path, optarg);
                break;
            case 'l':
                if (optarg) l = std::atoi(optarg);
                break;
            case 'r':
                if (optarg) r = std::atoi(optarg);
                break;
            case 'e':
                if (optarg) eps = std::atof(optarg);
                break;
            case 'm':
                if (optarg)strcpy(modularity_path, optarg);
                break;
        }
    }
    std::cerr<<"parameters:: L:"<<l<<" R:"<<r<<" Eps:"<<eps<<std::endl;
    auto *graph = new Graph(graph_path);
    graph->load_graph();
    graph->index_cluster_construct();
    std::cerr << "index construct" << std::endl;
    std::unordered_map<int, int> result_mp;
    graph->index_query_union(eps, l, r);
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
    double origin_modularity = compute_modularity(graph->graph_, result_mp, "origin");
    double split_modularity = compute_modularity(graph->graph_, result_mp, "split");

    std::cerr << "Origin Modularity:: " << origin_modularity << std::endl;
    std::cerr << "Split Modularity:: " << split_modularity << std::endl;
    LL rm_node = 0, rm_edge = 0;
    for (int i = 0; i < graph->graph_.size(); i++) {
        if (result_mp[i] == -1 || graph->graph_[i].empty()) {
            rm_node++;
            rm_edge += graph->graph_[i].size();
            continue;
        }
    }

    ofstream mout(modularity_path);
    mout << "Origin Modularity:: " << origin_modularity << std::endl;
    mout << "Split Modularity:: " << split_modularity << std::endl;
    mout << "Eliminate nodes:: " << rm_node <<" correspond %"<<100 * (double)rm_node/(double)graph->node_num<< std::endl;
    mout << "Eliminate edges:: " << rm_edge <<" correspond %"<<100 * (double)rm_edge/(double)graph->edge_num<< std::endl;
    mout.close();
    return 0;
}
/*
 */