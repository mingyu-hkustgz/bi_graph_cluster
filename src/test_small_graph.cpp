#include <iostream>
#include <fstream>
#include <ctime>
#include <cmath>
#include <utils.h>
#include <getopt.h>
#include "graph.h"

#define HASHMAP

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
    char result_path[256] = "";
    int left_miu = 1, right_miu = 1;
    float eps = 0.00;
    int method = 0;


    while (iarg != -1) {
        iarg = getopt_long(argc, argv, "g:l:r:e:i:m:s:", longopts, &ind);
        switch (iarg) {
            case 'g':
                if (optarg) strcpy(graph_path, optarg);
                break;
            case 'l':
                if (optarg) left_miu = atoi(optarg);
                break;
            case 'r':
                if (optarg) right_miu = atoi(optarg);
                break;
            case 'f':
                if (optarg) eps = atof(optarg);
                break;
            case 'i':
                if (optarg)strcpy(index_path, optarg);
                break;
            case 'm':
                if (optarg) method = atoi(optarg);
                break;
            case 's':
                if (optarg)strcpy(result_path, optarg);
                break;
        }
    }
    auto *graph = new Graph(graph_path);
    graph->generate_test_examples(100);
    if (method == 0) {
        graph->naive_cluster_construct();
        std::cerr << "index construct" << std::endl;
    }
    if (method == 1) {
        graph->index_cluster_construct();
        std::cerr << "index construct" << std::endl;
    }
    eps = 0.2;
    left_miu = 2;
    right_miu = 3;
    if (method == 0)
        graph->naive_query_union(eps, left_miu, right_miu);
    if (method == 1)
        graph->index_query_union(eps, left_miu, right_miu);
    std::cerr<<"finished"<<std::endl;
    graph->statistics_eps_per_edge(result_path);

    for(int i=0;i<graph->node_num;i++){
        if (graph->core_bm_[i]) {
            std::cerr << "core-> " << i<<" "<<graph->graph_[i].size() << std::endl;
        }
    }
    for(auto u:graph->result_non_core_){
        if(graph->core_bm_[u.second]) continue;
        std::cerr<<"no core: "<<graph->find_root(u.first)<<" "<<u.second<<std::endl;
    }


    return 0;
}
/*
 */