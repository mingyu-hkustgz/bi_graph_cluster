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
    char logger_path[256] = "";

    float eps = 0.00;
    int method = 0;
    int thread = 0;

    while (iarg != -1) {
        iarg = getopt_long(argc, argv, "g:l:r:e:i:m:s:t:", longopts, &ind);
        switch (iarg) {
            case 'g':
                if (optarg) strcpy(graph_path, optarg);
                break;
            case 'l':
                if (optarg) strcpy(logger_path, optarg);
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
            case 't':
                if (optarg) thread = atoi(optarg);
                break;
        }
    }
    auto *graph = new Graph(graph_path);
    graph->load_graph();
    auto s = std::chrono::high_resolution_clock::now();
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
        graph->naive_parallel_cluster_construct(thread);
        std::cerr << "index construct" << std::endl;
        graph->save_naive_data(index_path);
    }
    if (method == 3) {
        graph->index_parallel_cluster_construct(thread);
        std::cerr << "index construct" << std::endl;
        graph->save_index_data(index_path);
    }
    if (method == 4) {
        graph->index_parallel_cluster_construct(thread);
        std::cerr << "index construct" << std::endl;
        graph->save_index_data(index_path);
    }
    if (method ==5){
        graph->index_degree_cluster_construct();
        std::cerr << "index construct" << std::endl;
        graph->save_index_data(index_path);
    }

    auto e = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = e - s;
    double time_slap = diff.count();
    graph->statistics_eps_per_edge(logger_path);
    freopen(result_path, "a", stdout);
    printf("index time(s):: %.6f\n", time_slap);
    int quantile = 0;
    for (int i = (graph->stat_res.size() / 10); i < graph->stat_res.size(); i += (graph->stat_res.size() / 10)) {
        quantile++;
        printf("%d/10 :: sim-> %.6f\n", quantile, graph->stat_res[i]);
    }
    printf("max :: %.6f\n", graph->stat_res[graph->stat_res.size() - 1]);
    return 0;
}
