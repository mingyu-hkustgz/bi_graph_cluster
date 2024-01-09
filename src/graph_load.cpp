//
// Created by Mingyu on 24-1-7.
//

#include "graph.h"

void Graph::load_graph() {
    std::ifstream fin(graph_path);
    int x, y;
    fin >> left_nodes >> right_nodes >> node_num >> edge_num;
    node_num += 2;
    graph_.resize(node_num + 2);
    max_degree_ = 0;
    boost::unordered_map<std::pair<int, int>, bool, boost::hash<std::pair<int, int>>> check_tag;
    for (int i = 0; i < edge_num; i++) {
        fin >> x >> y;
        if (!check_tag[std::make_pair(x, y + left_nodes + 1)]) {
            graph_[x].push_back(y + left_nodes + 1);
            check_tag[std::make_pair(x, y + left_nodes + 1)] = true;
        }
        if (!check_tag[std::make_pair(y + left_nodes + 1, x)]) {
            graph_[y + left_nodes + 1].push_back(x);
            check_tag[std::make_pair(y + left_nodes + 1, x)] = true;
        }
    }
    for (int i = 0; i < node_num; i++) max_degree_ = std::max(max_degree_, (int) graph_[i].size());
    std::cerr << "load graph finished with node num:: " << node_num << " " << "edge num::" << edge_num << std::endl;
    node_two_hop_.resize(node_num, -1);
}

void Graph::save_naive_data(char *filename) {
    std::ofstream fout(filename, std::ios::binary);
    fout.write((char *) &node_num, sizeof(int));
    fout.write((char *) &edge_num, sizeof(int));
    fout.write((char *) &left_nodes, sizeof(int));
    fout.write((char *) &right_nodes, sizeof(int));
    fout.write((char *) &max_degree_, sizeof(int));
    for (int i = 0; i < node_num; i++) {
        unsigned size = graph_[i].size();
        fout.write((char *) &size, sizeof(unsigned));
        fout.write((char *) graph_[i].data(), sizeof(int) * graph_[i].size());
    }
    for (int i = 0; i < node_num; i++) {
        for (auto v: graph_[i]) {
            float similarity = similarity_square_[std::make_pair(i, v)];
            fout.write((char *) &similarity, sizeof(float));
        }
    }
}

void Graph::load_naive_data(char *filename) {
    std::ifstream fin(filename, std::ios::binary);
    fin.read((char *) &node_num, sizeof(int));
    fin.read((char *) &edge_num, sizeof(int));
    fin.read((char *) &left_nodes, sizeof(int));
    fin.read((char *) &right_nodes, sizeof(int));
    fin.read((char *) &max_degree_, sizeof(int));
    graph_.resize(node_num);
    for (int i = 0; i < node_num; i++) {
        unsigned size;
        fin.read((char *) &size, sizeof(unsigned));
        graph_[i].resize(size);
        fin.read((char *) graph_[i].data(), sizeof(int) * size);
    }
    for (int i = 0; i < node_num; i++) {
        for (auto v: graph_[i]) {
            float similarity;
            fin.read((char *) &similarity, sizeof(float));
            similarity_square_[std::make_pair(i, v)] = similarity;
            similarity_square_[std::make_pair(v, i)] = similarity;
        }
    }
}

void Graph::save_index_data(char *filename) {
    std::ofstream fout(filename, std::ios::binary);
    fout.write((char *) &node_num, sizeof(int));
    fout.write((char *) &edge_num, sizeof(int));
    fout.write((char *) &left_nodes, sizeof(int));
    fout.write((char *) &right_nodes, sizeof(int));
    fout.write((char *) &max_degree_, sizeof(int));
    for (int i = 0; i < node_num; i++) {
        unsigned size = graph_[i].size();
        fout.write((char *) &size, sizeof(unsigned));
        fout.write((char *) graph_[i].data(), sizeof(int) * size);
    }
    for (int i = 0; i < node_num; i++) {
        for (auto v: graph_[i]) {
            float similarity = similarity_square_[std::make_pair(i, v)];
            fout.write((char *) &similarity, sizeof(float));
        }
    }
    for (int i = 0; i < node_num; i++) {
        for (auto v: graph_[i]) {
            unsigned com_fly = common_bflys_[std::make_pair(i, v)];
            fout.write((char *) &com_fly, sizeof(int));
        }
    }
    fout.write((char *) index_core_cnt_left.data(), sizeof(int) * (max_degree_ + 1));
    fout.write((char *) index_core_cnt_right.data(), sizeof(int) * (max_degree_ + 1));
    for (int i = 1; i <= max_degree_; i++) {
        fout.write((char *) index_core_left[i].data(), sizeof(int) * index_core_cnt_left[i]);
        fout.write((char *) index_core_right[i].data(), sizeof(int) * index_core_cnt_right[i]);
    }
    fout.write((char *) node_two_hop_.data(), sizeof(LL) * node_num);
}


void Graph::load_index_data(char *filename) {
    std::ifstream fin(filename, std::ios::binary);
    fin.read((char *) &node_num, sizeof(int));
    fin.read((char *) &edge_num, sizeof(int));
    fin.read((char *) &left_nodes, sizeof(int));
    fin.read((char *) &right_nodes, sizeof(int));
    fin.read((char *) &max_degree_, sizeof(int));
    graph_.resize(node_num);
    for (int i = 0; i < node_num; i++) {
        unsigned size;
        fin.read((char *) &size, sizeof(unsigned));
        graph_[i].resize(size);
        fin.read((char *) graph_[i].data(), sizeof(int) * size);
    }
    for (int i = 0; i < node_num; i++) {
        for (auto v: graph_[i]) {
            float similarity;
            fin.read((char *) &similarity, sizeof(float));
            similarity_square_[std::make_pair(i, v)] = similarity;
            similarity_square_[std::make_pair(v, i)] = similarity;
        }
    }
    for (int i = 0; i < node_num; i++) {
        for (auto v: graph_[i]) {
            int com_fly;
            fin.read((char *) &com_fly, sizeof(int));
            common_bflys_[std::make_pair(i, v)] = com_fly;
            common_bflys_[std::make_pair(v, i)] = com_fly;
        }
    }
    index_core_cnt_left.resize(max_degree_ + 1);
    index_core_cnt_right.resize(max_degree_ + 1);
    fin.read((char *) index_core_cnt_left.data(), sizeof(int) * (max_degree_ + 1));
    fin.read((char *) index_core_cnt_right.data(), sizeof(int) * (max_degree_ + 1));
    index_core_left.resize(max_degree_ + 1);
    index_core_right.resize(max_degree_ + 1);
    for (int i = 1; i <= max_degree_; i++) {
        index_core_left[i].resize(index_core_cnt_left[i]);
        index_core_right[i].resize(index_core_cnt_right[i]);
        fin.read((char *) index_core_left[i].data(), sizeof(int) * index_core_cnt_left[i]);
        fin.read((char *) index_core_right[i].data(), sizeof(int) * index_core_cnt_right[i]);
    }
    node_two_hop_.resize(node_num);
    fin.read((char *) node_two_hop_.data(), sizeof(LL) * node_num);
}