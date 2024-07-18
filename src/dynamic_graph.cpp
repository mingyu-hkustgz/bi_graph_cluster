#include "graph.h"

void Graph::dynamic_index_init(char *filename) {
    std::ifstream fin(filename, std::ios::binary);
    fin.read((char *) &node_num, sizeof(int));
    fin.read((char *) &edge_num, sizeof(int));
    fin.read((char *) &left_nodes, sizeof(int));
    fin.read((char *) &right_nodes, sizeof(int));
    fin.read((char *) &max_degree_, sizeof(int));

    cores_mp_left_.resize(max_degree_ + 1, std::multimap<float, int, std::greater<>>());
    cores_mp_right_.resize(max_degree_ + 1, std::multimap<float, int, std::greater<>>());
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
        for (int j = 0; j < index_core_cnt_left[i]; j++) {
            unsigned id = index_core_left[i][j];
            float similarity = similarity_square_[std::make_pair(id, graph_[id][i - 1])];
            cores_mp_left_[i].emplace(similarity, id);
        }
        for (int j = 0; j < index_core_cnt_right[i]; j++) {
            unsigned id = index_core_right[i][j];
            float similarity = similarity_square_[std::make_pair(id, graph_[id][i - 1])];
            cores_mp_right_[i].emplace(similarity, id);
        }
    }
    for (int i = 0; i < node_num; i++) {
        for (auto v: graph_[i]) {
            LL com_fly;
            fin.read((char *) &com_fly, sizeof(LL));
            common_bflys_[std::make_pair(i, v)] = com_fly;
        }
    }
}


void Graph::recompute_edge_similarity(int u, int v) {
    LL uv_wedge = ((LL) graph_[u].size() - 1) * ((LL) graph_[v].size() - 1) + 1;
    LL cn = common_bflys_[std::make_pair(u, v)];
    float tmp_eps = (long double) (cn + 1) / uv_wedge;
    similarity_square_[std::make_pair(u, v)] = tmp_eps;
    similarity_square_[std::make_pair(v, u)] = tmp_eps;
}


void Graph::fast_insert_edge(int u, int v) {
    int left = std::min(u, v), right = std::max(u, v);
    if (left > left_nodes || right <= left_nodes) return;
    for (auto neighbor: graph_[u]) {
        if (neighbor == v) return;
    }
    boost::unordered_map<std::pair<int, int>, bool, boost::hash<std::pair<int, int> > > common_edges_;
    for (auto neighbor: graph_[u]) {
        for (auto two_hop: graph_[neighbor]) {
            if (two_hop != u) {
                common_edges_[std::make_pair(neighbor, two_hop)] = true;
                common_edges_[std::make_pair(two_hop, neighbor)] = true;
            }
        }
    }
    std::vector<std::pair<int, int> > influenced_pair;
    influenced_pair.emplace_back(u, v);
    for (auto next_u: graph_[u]) influenced_pair.emplace_back(u, next_u);
    for (auto next_v: graph_[v]) influenced_pair.emplace_back(v, next_v);
    for (auto next_u: graph_[u]) {
        for (auto next_v: graph_[v]) {
            if (common_edges_[std::make_pair(next_u, next_v)]) {
                common_bflys_[std::make_pair(next_u, next_v)]++;
                common_bflys_[std::make_pair(next_v, next_u)]++;
                influenced_pair.emplace_back(next_u, next_v);

                common_bflys_[std::make_pair(u, next_u)]++;
                common_bflys_[std::make_pair(next_u, u)]++;

                common_bflys_[std::make_pair(next_v, v)]++;
                common_bflys_[std::make_pair(v, next_v)]++;
            }

        }
    }
    // remove influenced similarity
    remove_influenced_node(u, v);
    graph_[u].push_back(v);
    graph_[v].push_back(u);
    LL new_btf = 0;
    std::unordered_map<int, int> two_hop_map;
    get_two_hop_map(u, two_hop_map);
    new_btf = fast_compute_common_bflys(u, v, two_hop_map);
    common_bflys_[std::make_pair(u, v)] = new_btf;
    common_bflys_[std::make_pair(v, u)] = new_btf;

    for (auto item: influenced_pair) {
        recompute_edge_similarity(item.first, item.second);
    }

    // reorder similarity index
    reorder_index(u, v);
}

void Graph::fast_delete_edge(int u, int v) {
    int left = std::min(u, v), right = std::max(u, v);
    if (left > left_nodes || right <= left_nodes) return;
    bool check = false;
    for (auto neighbor: graph_[u]) {
        if (neighbor == v) {
            check = true;
            break;
        }
    }
    if (!check) return;

    //remove influenced similarity
    remove_influenced_node(u, v);
    similarity_square_.erase(std::make_pair(u, v));
    similarity_square_.erase(std::make_pair(v, u));
    common_bflys_.erase(std::make_pair(u, v));
    common_bflys_.erase(std::make_pair(v, u));

    unsigned size = graph_[u].size();
    for (int i = 0; i < size; i++) {
        if (graph_[u][i] == v) {
            std::swap(graph_[u][i], graph_[u][size - 1]);
            graph_[u].pop_back();
            break;
        }
    }

    size = graph_[v].size();
    for (int i = 0; i < graph_[v].size(); i++) {
        if (graph_[v][i] == u) {
            std::swap(graph_[v][i], graph_[v][size - 1]);
            graph_[v].pop_back();
            break;
        }
    }

    boost::unordered_map<std::pair<int, int>, bool, boost::hash<std::pair<int, int> > > common_edges_;
    for (auto neighbor: graph_[u]) {
        for (auto two_hop: graph_[neighbor]) {
            if (two_hop != u) {
                common_edges_[std::make_pair(neighbor, two_hop)] = true;
                common_edges_[std::make_pair(two_hop, neighbor)] = true;
            }
        }
    }
    std::vector<std::pair<int, int> > influenced_pair;
    for (auto next_u: graph_[u]) influenced_pair.emplace_back(u, next_u);
    for (auto next_v: graph_[v]) influenced_pair.emplace_back(v, next_v);
    for (auto next_u: graph_[u]) {
        for (auto next_v: graph_[v]) {
            if (common_edges_[std::make_pair(next_u, next_v)]) {

                common_bflys_[std::make_pair(next_u, next_v)]--;
                common_bflys_[std::make_pair(next_v, next_u)]--;
                influenced_pair.emplace_back(next_u, next_v);

                common_bflys_[std::make_pair(u, next_u)]--;
                common_bflys_[std::make_pair(next_u, u)]--;

                common_bflys_[std::make_pair(next_v, v)]--;
                common_bflys_[std::make_pair(v, next_v)]--;
            }
        }
    }

    for (auto item: influenced_pair) {
        recompute_edge_similarity(item.first, item.second);
    }
    // reorder similarity index
    reorder_index(u, v);
}

void Graph::remove_influenced_node(int u, int v) {
    std::vector<int> influenced_node;
    influenced_node.push_back(u);
    influenced_node.push_back(v);
    for (auto neighbor: graph_[u]) influenced_node.push_back(neighbor);
    for (auto neighbor: graph_[v]) influenced_node.push_back(neighbor);

    for (auto node: influenced_node) {
        for (int j = 0; j < graph_[node].size(); j++) {
            int deg = j + 1;
            int neighbor = graph_[node][j];
            if (node <= left_nodes) {
                float similarity = similarity_square_[std::make_pair(neighbor, node)];
                auto range = cores_mp_left_[deg].equal_range(similarity);
                for (auto it = range.first; it != range.second;) {
                    if (it->second == node) {
                        it = cores_mp_left_[deg].erase(it);
                    } else {
                        ++it;
                    }
                }
            } else {
                float similarity = similarity_square_[std::make_pair(neighbor, node)];
                auto range = cores_mp_right_[deg].equal_range(similarity);
                for (auto it = range.first; it != range.second;) {
                    if (it->second == node) {
                        it = cores_mp_right_[deg].erase(it);
                    } else {
                        ++it;
                    }
                }
            }
        }
    }
}


void Graph::reorder_index(int u, int v) {
    if (u > v) std::swap(u, v);
    if (graph_[u].size() > max_degree_) {
        max_degree_ = graph_[u].size();
        cores_mp_left_.resize(max_degree_ + 1);
        cores_mp_right_.resize(max_degree_ + 1);
    }

    if (graph_[v].size() > max_degree_) {
        max_degree_ = graph_[v].size();
        cores_mp_left_.resize(max_degree_ + 1);
        cores_mp_right_.resize(max_degree_ + 1);
    }

    std::vector<int> influenced_node;
    influenced_node.push_back(u);
    influenced_node.push_back(v);
    for (auto neighbor: graph_[u]) if (neighbor != v) influenced_node.push_back(neighbor);
    for (auto neighbor: graph_[v]) if (neighbor != u) influenced_node.push_back(neighbor);

    for (auto node: influenced_node) {
        sort_nbr_by_similarity(node);
        for (int j = 0; j < graph_[node].size(); j++) {
            int cur_deg = j + 1;
            int neighbor = graph_[node][j];
            float similarity = similarity_square_[std::make_pair(node, neighbor)];
            if (node <= left_nodes) {
                cores_mp_left_[cur_deg].emplace(similarity, node);
            } else {
                cores_mp_right_[cur_deg].emplace(similarity, node);
            }
        }
    }
}


void Graph::naive_insert_edge(int u, int v) {
    int left = std::min(u, v), right = std::max(u, v);
    if (left > left_nodes || right <= left_nodes) return;
    for (auto neighbor: graph_[u]) {
        if (neighbor == v) return;
    }
    boost::unordered_map<std::pair<int, int>, bool, boost::hash<std::pair<int, int> > > common_edges_;
    for (auto neighbor: graph_[u]) {
        for (auto two_hop: graph_[neighbor]) {
            if (two_hop != u) {
                common_edges_[std::make_pair(neighbor, two_hop)] = true;
                common_edges_[std::make_pair(two_hop, neighbor)] = true;
            }
        }
    }
    std::vector<std::pair<int, int> > influenced_pair;
    influenced_pair.emplace_back(u, v);
    for (auto next_u: graph_[u]) influenced_pair.emplace_back(u, next_u);
    for (auto next_v: graph_[v]) influenced_pair.emplace_back(v, next_v);
    for (auto next_u: graph_[u]) {
        for (auto next_v: graph_[v]) {
            if (common_edges_[std::make_pair(next_u, next_v)]) {
                influenced_pair.emplace_back(next_u, next_v);
            }
        }
    }
    // remove influenced similarity
    remove_influenced_node(u, v);
    graph_[u].push_back(v);
    graph_[v].push_back(u);
    for (auto item: influenced_pair) {
        LL new_btf = 0;
        std::unordered_map<int, int> two_hop_map;
        get_two_hop_map(item.first, two_hop_map);
        new_btf = fast_compute_common_bflys(item.first, item.second, two_hop_map);
        common_bflys_[std::make_pair(item.first, item.second)] = new_btf;
        common_bflys_[std::make_pair(item.second, item.first)] = new_btf;
        recompute_edge_similarity(item.first, item.second);
    }
    // reorder similarity index
    reorder_index(u, v);
}

void Graph::naive_delete_edge(int u, int v) {
    int left = std::min(u, v), right = std::max(u, v);
    if (left > left_nodes || right <= left_nodes) return;
    bool check = false;
    for (auto neighbor: graph_[u]) {
        if (neighbor == v) {
            check = true;
            break;
        }
    }
    if (!check) return;

    //remove influenced similarity
    remove_influenced_node(u, v);
    similarity_square_.erase(std::make_pair(u, v));
    similarity_square_.erase(std::make_pair(v, u));
    common_bflys_.erase(std::make_pair(u, v));
    common_bflys_.erase(std::make_pair(v, u));

    unsigned size = graph_[u].size();
    for (int i = 0; i < size; i++) {
        if (graph_[u][i] == v) {
            std::swap(graph_[u][i], graph_[u][size - 1]);
            graph_[u].pop_back();
            break;
        }
    }

    size = graph_[v].size();
    for (int i = 0; i < graph_[v].size(); i++) {
        if (graph_[v][i] == u) {
            std::swap(graph_[v][i], graph_[v][size - 1]);
            graph_[v].pop_back();
            break;
        }
    }

    boost::unordered_map<std::pair<int, int>, bool, boost::hash<std::pair<int, int> > > common_edges_;
    for (auto neighbor: graph_[u]) {
        for (auto two_hop: graph_[neighbor]) {
            if (two_hop != u) {
                common_edges_[std::make_pair(neighbor, two_hop)] = true;
                common_edges_[std::make_pair(two_hop, neighbor)] = true;
            }
        }
    }
    std::vector<std::pair<int, int> > influenced_pair;
    for (auto next_u: graph_[u]) influenced_pair.emplace_back(u, next_u);
    for (auto next_v: graph_[v]) influenced_pair.emplace_back(v, next_v);
    for (auto next_u: graph_[u]) {
        for (auto next_v: graph_[v]) {
            if (common_edges_[std::make_pair(next_u, next_v)]) {
                influenced_pair.emplace_back(next_u, next_v);
            }
        }
    }

    for (auto item: influenced_pair) {
        LL new_btf = 0;
        std::unordered_map<int, int> two_hop_map;
        get_two_hop_map(item.first, two_hop_map);
        new_btf = fast_compute_common_bflys(item.first, item.second, two_hop_map);
        common_bflys_[std::make_pair(item.first, item.second)] = new_btf;
        common_bflys_[std::make_pair(item.second, item.first)] = new_btf;
        recompute_edge_similarity(item.first, item.second);
    }
    // reorder similarity index
    reorder_index(u, v);
}


int Graph::find_reverse_top(int &node, int &neighbor, float &similarity) {
    if (graph_[node].size() < 10) {
        for (int j = 0; j < graph_[node].size(); j++) {
            if (graph_[node][j] == neighbor) return j + 1;
        }
        return -1;
    }

    int l = 0, r = (int) graph_[node].size() - 1, res;
    while (l <= r) {
        int mid = (l + r) >> 1;
        if (similarity_square_[std::make_pair(node, graph_[node][mid])] < similarity) {
            l = mid + 1;
            res = mid;
        } else r = mid - 1;
    }
    while (++res && res < graph_[node].size()) {
        if (graph_[node][res] == neighbor) {
            return res + 1;
        }
    }
    return -1;
}


bool Graph::neighbor_exist(int &node, int &neighbor) {
    for (auto node_next: graph_[node]) {
        if (node_next == neighbor) return true;
    }
    return false;
}
