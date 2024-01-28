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
        iarg = getopt_long(argc, argv, "g:i:m:s:", longopts, &ind);
        switch (iarg) {
            case 'g':
                if (optarg) strcpy(graph_path, optarg);
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
    if (method == 0)
        graph->load_naive_data(index_path);
    if (method == 1)
        graph->load_index_data(index_path);

    freopen(result_path, "a", stdout);
    std::cout.setf(std::ios::fixed);
    std::cout.precision(8);
    std::vector<double> query_var_eps_time, query_var_lmiu_time, query_var_rmiu_time;
    double ave_left_node,ave_right_node;
    ave_left_node = graph->get_ave_left_degree();
    ave_right_node = graph->get_ave_right_degree();
    left_miu = ave_left_node + 0.5;
    right_miu = ave_right_node + 0.5;
    printf("nodes:: %d left nodes:: %d right nodes:: %d ave left degree %.4f ave right degree %.4f",
           graph->node_num,graph->left_nodes,graph->right_nodes,ave_left_node,ave_right_node);
    for(int i=1;i<=5;i++){
        eps = 0.2*i;
        auto s = std::chrono::high_resolution_clock::now();
        if (method == 0)
            graph->naive_query_union(eps, left_miu, right_miu);
        if (method == 1)
            graph->index_query_union(eps, left_miu, right_miu);
        auto e = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = e - s;
        double time_slap = diff.count();
        query_var_eps_time.push_back(time_slap);
    }
    for(int tmp_left=2;tmp_left<=20;tmp_left+=2){
        eps=0.5;
        auto s = std::chrono::high_resolution_clock::now();
        if (method == 0)
            graph->naive_query_union(eps, tmp_left, right_miu);
        if (method == 1)
            graph->index_query_union(eps, tmp_left, right_miu);
        auto e = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = e - s;
        double time_slap = diff.count();
        query_var_lmiu_time.push_back(time_slap);
    }
    for(int tmp_right=2;tmp_right<=20;tmp_right+=2){
        eps=0.5;
        auto s = std::chrono::high_resolution_clock::now();
        if (method == 0)
            graph->naive_query_union(eps, left_miu, tmp_right);
        if (method == 1)
            graph->index_query_union(eps, left_miu, tmp_right);
        auto e = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = e - s;
        double time_slap = diff.count();
        query_var_rmiu_time.push_back(time_slap);
    }
    if(method==0) printf("naive result::\n");
    else printf("index result::\n");
    for(int i=1;i<=5;i++){
        printf("eps:: %.6f lmiu %d rmiu %d time(s) %.6f\n",0.2*i, left_miu, right_miu, query_var_eps_time[i-1]);
    }

    for(int i=1;i<=5;i++) printf("%.6f ",query_var_eps_time[i-1]);
    printf("\n");

    for(int i=1;i<=10;i++){
        printf("eps:: %.6f lmiu %d rmiu %d time(s) %.6f\n",0.5, i*2, right_miu, query_var_lmiu_time[i-1]);
    }

    for(int i=1;i<=10;i++) printf("%.6f ",query_var_lmiu_time[i-1]);
    printf("\n");

    for(int i=1;i<=10;i++){
        printf("eps:: %.6f lmiu %d rmiu %d time(s) %.6f\n",0.5, left_miu, i*2, query_var_rmiu_time[i-1]);
    }

    for(int i=1;i<=10;i++) printf("%.6f ",query_var_rmiu_time[i-1]);
    printf("\n");

    return 0;
}