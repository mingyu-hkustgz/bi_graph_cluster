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
            case 'e':
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
    graph->load_graph();
    if (method == 0) {
        if(isFileExists_ifstream(index_path))
            graph->load_naive_data(index_path);
        else{
            graph->naive_cluster_construct();
            graph->save_naive_data(index_path);
        }
        std::cerr << "naive construct" << std::endl;
    }
    if (method == 1) {
        if(isFileExists_ifstream(index_path))
            graph->load_index_data(index_path);
        else{
            graph->index_cluster_construct();
            graph->save_index_data(index_path);
        }
        std::cerr << "index construct" << std::endl;
    }

    if (method == 0)
        graph->naive_query_union(eps, left_miu, right_miu);
    if (method == 1)
        graph->index_query_union(eps, left_miu, right_miu);

    graph->statistics_eps_per_edge("test.log");
   
    std::ofstream fout(result_path, std::ios::binary);
    int left_num = graph->left_nodes+1;
    fout.write((char*)&left_num,sizeof(int));
    for(int i=0;i<left_num;i++){
        int result = graph->find_root(i);
        fout.write((char*)&result,sizeof(int));
    }

    return 0;
}
/*
 */