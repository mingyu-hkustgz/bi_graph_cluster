//
// Created by BLD on 24-7-14.
//
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

    int ind, method;
    int iarg = 0;
    opterr = 1;    //getopt error message (off: 0)
    char graph_path[256] = "";
    char index_path[256] = "";
    char result_path[256] = "";
    int query_num = 1000;

    while (iarg != -1) {
        iarg = getopt_long(argc, argv, "g:i:m:r:t:", longopts, &ind);
        switch (iarg) {
            case 'g':
                if (optarg) strcpy(graph_path, optarg);
                break;
            case 'i':
                if (optarg) strcpy(index_path, optarg);
                break;
            case 'r':
                if (optarg) strcpy(result_path, optarg);
                break;
            case 'm':
                if (optarg) method = atoi(optarg);
                break;
            case 't':
                if (optarg) query_num = atoi(optarg);
                break;
        }
    }
    auto *graph_base = new Graph(graph_path);
    auto *graph_dynamic = new Graph(graph_path);
    graph_base->load_graph();
    graph_base->index_cluster_construct();
    graph_base->save_index_data(index_path, true);
    graph_dynamic->dynamic_index_init(index_path);
    std::cerr << "dynamic index init finished" << std::endl;
    freopen(result_path, "a", stdout);
    srand(123);
    double dynamic_time = 0.0;
    if (query_num > graph_dynamic->edge_num) query_num = graph_dynamic->edge_num >> 1;

//    std::vector<std::pair<int, int> > node_order_list;
//    for (int i = 0; i < graph_dynamic->node_num; i++) node_order_list.emplace_back(-graph_dynamic->graph_[i].size(), i);
//    std::sort(node_order_list.begin(), node_order_list.end());

    for (int i = 0; i < query_num; i++) {
        int u = rand() % graph_dynamic->left_nodes + 1;
        int v = rand() % graph_dynamic->right_nodes + graph_dynamic->left_nodes + 1;
        if (method == 0 || method == 1) {
            while (graph_dynamic->neighbor_exist(u, v)) {
                u = rand() % graph_dynamic->left_nodes + 1;
                v = rand() % graph_dynamic->right_nodes + graph_dynamic->left_nodes + 1;
            }
        } else {
            while (graph_dynamic->graph_[u].empty()) {
                u = rand() % graph_dynamic->left_nodes + 1;
            }
            int nxt_idx = rand() % graph_dynamic->graph_[u].size();
            v = graph_dynamic->graph_[u][nxt_idx];
        }
        auto s = std::chrono::high_resolution_clock::now();
        if (method == 0) {
            graph_dynamic->naive_insert_edge(u, v);
        } else if (method == 1) {
            graph_dynamic->fast_insert_edge(u, v);
        } else if (method == 2) {
            graph_dynamic->naive_delete_edge(u, v);
        } else if (method == 3) {
            graph_dynamic->fast_delete_edge(u, v);
        }
        auto e = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = e - s;
        double time_slap = diff.count();
        dynamic_time += time_slap;
    }
    dynamic_time /= (double) query_num;
    if (method == 0) {
        printf("naive insert time %.8f\n", dynamic_time);
    } else if (method == 1) {
        printf("fast insert time %.8f\n", dynamic_time);
    } else if (method == 2) {
        printf("naive delete time %.8f\n", dynamic_time);
    } else if (method == 3) {
        printf("fast delete time %.8f\n", dynamic_time);
    }
    return 0;
}
