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
    char result_path[256] = "";
    char logger_path[256] = "";
    char weight_path[256] = "";
    int left_miu = 1, right_miu = 1;
    float eps = 0.00;
    int method = 0;


    while (iarg != -1) {
        iarg = getopt_long(argc, argv, "g:l:r:e:i:m:s:d:w:", longopts, &ind);
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
            case 'd':
                if (optarg)strcpy(logger_path, optarg);
                break;
            case 'w':
                if (optarg)strcpy(weight_path, optarg);
                break;
        }
    }
    auto *graph = new Graph(graph_path);
    graph->load_graph();
    if (method == 0) {
        graph->naive_cluster_construct();
        std::cerr << "naive construct" << std::endl;
    }
    if (method == 1) {
        graph->index_cluster_construct();
        std::cerr << "index construct" << std::endl;
    }
    if (method == 2) {
        graph->naive_reconstruct_cluster_construct();
        std::cerr << "reconstruct construct" << std::endl;
    }
    graph->statistics_eps_per_edge(logger_path);
    if (method == 0)
        graph->naive_query_union(eps, left_miu, right_miu);
    if (method == 1)
        graph->index_query_union(eps, left_miu, right_miu);
    if (method == 2)
        graph->reconstruct_query_union(eps, left_miu);

   
    std::ofstream fout(result_path);
    fout<<"id,tag"<<endl;
    std::map<int,int> cluster_id_map;
    int count = 1;
    int count_core = 0;
   for(int i=0;i<graph->node_num;i++){
       if(graph->graph_[i].empty()) continue;
       if(graph->core_bm_[i]) count_core++;
       if(graph->core_bm_[i])
       std::cerr<<"core:: "<<i<<" "<<graph->core_bm_[i]<<" "<<graph->graph_[i].size()<<std::endl;
       int result = graph->fa_[i];
         if(result==i && !graph->core_bm_[i]) result=-1;
         if(cluster_id_map[result]==0){
             cluster_id_map[result] = count;
             count++;
         }
         fout<<i<<","<<result<<endl;
    }
   std::cerr<<count<<endl;
   std::cerr<<count_core<<endl;
    graph->save_similarity_edge(weight_path);
    return 0;
}
/*
 */