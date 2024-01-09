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
    if(isFileExists_ifstream(index_path)){
        graph->generate_test_examples(20);
        graph->index_cluster_construct();
        graph->save_index_data(index_path);
    }else{
        graph->dynamic_index_init(index_path);
    }

    std::cerr << "index construct" << std::endl;
    graph->index_query_union(eps, left_miu, right_miu);
    std::cerr<<"finished"<<std::endl;

    return 0;
}
/*
 *
 */