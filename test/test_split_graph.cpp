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
        iarg = getopt_long(argc, argv, "g:s:", longopts, &ind);
        switch (iarg) {
            case 'g':
                if (optarg) strcpy(graph_path, optarg);
                break;
            case 's':
                if (optarg)strcpy(result_path, optarg);
                break;
        }
    }
    auto *graph = new Graph(graph_path);
    graph->load_graph();
    freopen(result_path, "a", stdout);
    std::cout<<"graph:: "<<graph_path<<std::endl;
    std::cout<<"nodes:: "<<graph->node_num-2<<" left nodes:: "<<graph->left_nodes<<" right nodes:: "<<graph->right_nodes<<" edges:: "<<graph->edge_num<<std::endl;
    std::cout<<"left ave degree "<<graph->get_ave_left_degree()<<std::endl;
    std::cout<<"right ave degree "<<graph->get_ave_right_degree()<<std::endl;
    return 0;
}
/*
 *
 */