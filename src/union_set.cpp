#include "graph.h"

int Graph::find_root(int u) const {
    int x = u;
    while (fa_[x] != x) x = fa_[x];
    while (fa_[u] != x) {
        int tmp = fa_[u];
        fa_[u] = x;
        u = tmp;
    }
    return x;
}

void Graph::set_union(int u, int v) const {
    int ru = find_root(u);
    int rv = find_root(v);
    if (ru == rv) return;
    fa_[ru] = rv;
}