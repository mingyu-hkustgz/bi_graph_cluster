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

    int ind;
    int iarg = 0;
    opterr = 1;    //getopt error message (off: 0)
    char graph_path[256] = "";
    char index_path[256] = "";
    int query_num = 1000;

    while (iarg != -1) {
        iarg = getopt_long(argc, argv, "g:i:t:", longopts, &ind);
        switch (iarg) {
            case 'g':
                if (optarg) strcpy(graph_path, optarg);
                break;
            case 'i':
                if (optarg) strcpy(index_path, optarg);
                break;
            case 't':
                if (optarg) query_num = atoi(optarg);
                break;
        }
    }
    auto *graph_base = new Graph(graph_path);
    auto *graph_dynamic = new Graph(graph_path);
    auto *graph_naive = new Graph(graph_path);
    graph_base->load_graph();
    graph_base->index_cluster_construct();
    graph_base->save_index_data(index_path, true);


    graph_dynamic->dynamic_index_init(index_path);
    std::cerr << "dynamic index init finished" << std::endl;
    srand(123);
    while (query_num--) {
        int u = rand() % graph_dynamic->left_nodes + 1;
        int v = rand() % graph_dynamic->right_nodes + 1 + graph_dynamic->left_nodes;
        if (query_num & 1)
            graph_dynamic->naive_delete_edge(u, v);
        else
            graph_dynamic->naive_insert_edge(u, v);
    }
    graph_naive->left_nodes = graph_dynamic->left_nodes;
    graph_naive->right_nodes = graph_dynamic->right_nodes;
    graph_naive->node_num = graph_dynamic->node_num;
    graph_naive->graph_ = graph_dynamic->graph_;
    graph_naive->max_degree_ = graph_dynamic->max_degree_;
    graph_naive->index_cluster_construct();

    // Test Similarity
    for (int i = 0; i < graph_naive->node_num; i++) {
        for (auto next: graph_naive->graph_[i]) {
            float sim_naive = graph_naive->similarity_square_[std::make_pair(i, next)];
            float sim_dynamic = graph_dynamic->similarity_square_[std::make_pair(i, next)];
            if (sim_dynamic != sim_naive) {
                std::cerr << "ERROR SIM: " << sim_dynamic << " " << sim_naive << " " << i << " " << next << std::endl;
                std::cerr << "BUTTERFLY NAIVE:: " << graph_naive->common_bflys_[std::make_pair(i, next)] << std::endl;
                std::cerr << "BUTTERFLY DYNAMIC:: " << graph_dynamic->common_bflys_[std::make_pair(i, next)]
                          << std::endl;
            }
        }
    }

    // Test core order
    for (int i = 1; i <= graph_dynamic->max_degree_; i++) {
        int cnt = 0;
        for (auto item: graph_dynamic->cores_mp_left_[i]) {
            int node = graph_naive->index_core_left[i][cnt];
            int neighbor = graph_naive->graph_[node][i - 1];
            float similarity = graph_naive->similarity_square_[std::make_pair(node, neighbor)];
            if (similarity != item.first) {
                std::cerr << "ERROR LEFT ORDER:: " << i << " " << cnt << " " << similarity << " " << item.first
                          << std::endl;
            }
            cnt++;
        }
        cnt = 0;
        for (auto item: graph_dynamic->cores_mp_right_[i]) {
            int node = graph_naive->index_core_right[i][cnt];
            int neighbor = graph_naive->graph_[node][i - 1];
            float similarity = graph_naive->similarity_square_[std::make_pair(node, neighbor)];
            if (similarity != item.first) {
                std::cerr << "ERROR RIGHT ORDER:: " << i << " " << cnt << " " << similarity << " " << item.first
                          << std::endl;
            }
            cnt++;
        }

    }


    std::cerr << "static index finished" << std::endl;
    graph_naive->index_query_union(0.5, 2, 2);
    graph_dynamic->dynamic_query_union(0.5, 2, 2);
    std::cerr << "query finished" << std::endl;


    for (int i = 0; i < graph_naive->node_num; i++) {
        if (graph_dynamic->core_bm_[i] != graph_naive->core_bm_[i]) {
            std::cerr << "error at-> " << i << std::endl;
            std::cerr << graph_dynamic->core_bm_[i] << " " << graph_naive->core_bm_[i] << std::endl;
            for (auto u: graph_naive->graph_[i]) {
                float similarity = graph_naive->similarity_square_[std::make_pair(i, u)];
                std::cerr << "edge " << u << "-" << i << " " << similarity << std::endl;
            }
            std::cerr << "---------------------" << std::endl;
            for (auto u: graph_dynamic->graph_[i]) {
                float similarity = graph_dynamic->similarity_square_[std::make_pair(i, u)];
                std::cerr << "edge " << u << "-" << i << " " << similarity << std::endl;
            }


        }
    }

    return 0;
}
