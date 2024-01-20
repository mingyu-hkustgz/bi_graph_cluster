#include <iostream>
#include <fstream>
#include <ctime>
#include <cmath>
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
    if (!isFileExists_ifstream(index_path)) {
        graph->load_graph();
        if (method == 0) {
            graph->naive_cluster_construct();
            std::cerr << "index construct" << std::endl;
            graph->save_naive_data(index_path);
        }
        if (method == 1) {
            graph->index_cluster_construct();
            std::cerr << "index construct" << std::endl;
            graph->save_index_data(index_path);
        }
        if (method == 2) {
            graph->naive_parallel_cluster_construct(120);
            std::cerr << "index construct" << std::endl;
            graph->save_naive_data(index_path);
        }
        if (method == 3) {
            graph->index_parallel_cluster_construct(120);
            std::cerr << "index construct" << std::endl;
            graph->save_index_data(index_path);
        }
    } else {
        if (method == 0)
            graph->load_naive_data(index_path);
        if (method == 1)
            graph->load_index_data(index_path);
        if (method == 2)
            graph->load_naive_data(index_path);
        if (method == 3)
            graph->load_index_data(index_path);
        std::cerr << "index load" << std::endl;
    }
    if (method == 0)
        graph->naive_query_union(eps, left_miu, right_miu);
    if (method == 1)
        graph->index_query_union(eps, left_miu, right_miu);
    if (method == 2)
        graph->naive_query_union(eps, left_miu, right_miu);
    if (method == 3)
        graph->index_query_union(eps, left_miu, right_miu);
    graph->statistics_eps_per_edge(result_path);
    std::cerr<<left_miu<<" "<<right_miu<<std::endl;
    for (int i = 0; i <10000; i++) {
        if (graph->find_root(i) == i && graph->core_bm_[i]) {
            std::cerr << "root-> " << i<<" "<<graph->graph_[i].size() << std::endl;
        }
    }
    return 0;
}
/*
root-> 9 82
root-> 272 156
root-> 452 136
root-> 1059 94
root-> 1481 272
root-> 1588 93
root-> 1930 154
root-> 2086 82
root-> 2351 114
root-> 2544 97
root-> 2655 234
root-> 3283 153
root-> 3439 173
root-> 3513 310
root-> 3537 108
root-> 3726 101
root-> 4099 105
root-> 4299 86
root-> 5016 107
root-> 5322 83
root-> 5533 148
root-> 5566 126
root-> 5625 85
root-> 6273 105
root-> 6328 394
root-> 6871 87
root-> 7183 96
root-> 7410 80
root-> 7568 88
root-> 7603 104
root-> 7640 287
root-> 7969 86
root-> 8136 123
root-> 8196 82
root-> 8430 99
root-> 8470 130
root-> 8597 86
root-> 8804 250
root-> 9235 111
root-> 9353 257
 */