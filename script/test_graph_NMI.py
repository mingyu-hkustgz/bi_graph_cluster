import os.path

import networkx as nx
import community
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import numpy as np
import scipy.io as scio
from sklearn.cluster import SpectralClustering
from sklearn.cluster import SpectralCoclustering
from sklearn.cluster import SpectralBiclustering
from utils import *
from sklearn.metrics.cluster import normalized_mutual_info_score
from sklearn.cluster import KMeans
from test_graph_modularity import *

datasets = ["WebKB_wisconsin", "WebKB_washington", "WebKB_texas", "IMDb_movies_actors", "IMDb_movies_keywords",
            "WebKB_cornell"]
num_clusters = {
    "WebKB_wisconsin": 5,
    "WebKB_washington": 5,
    "WebKB_texas": 5,
    "WebKB_cornell": 5,
    "IMDb_movies_actors": 17,
    "IMDb_movies_keywords": 17,
}


def load_matrix(fname):
    matrix = scio.loadmat(fname)
    return matrix["fea"], matrix["gnd"]


def eval(tru, pre):
    # true label: numpy, vector in col
    # pred lable: numpy, vector in row

    num_labels = tru.shape[0]
    # nmi
    nmi = normalized_mutual_info_score(tru.reshape(num_labels), pre)

    # accuracy
    tru = np.reshape(tru, (num_labels))
    # set_tru = set(tru.tolist())
    set_pre = set(pre.tolist())
    # nclu_tru = len(set_tru) # in case that nclu_tru != the preset cluster num
    nclu_pre = len(set_pre)

    correct = 0
    for i in range(nclu_pre):
        flag = list(set_pre)[i]
        idx = np.argwhere(pre == flag)
        correct += max(np.bincount(tru[idx].T[0].astype(np.int64)))
    acc = correct / num_labels

    return acc, nmi


def save_edges(edges, fname, left_nodes, right_nodes, node_num, edge_num):
    print(f"{left_nodes} {right_nodes} {node_num} {edge_num}", file=open(fname, 'a'))
    for item in edges:
        print(f"{item[0]} {item[1]}", file=open(fname, "a"))


if __name__ == "__main__":
    for dataset in datasets:
        file_path = f"./DATA/mat/{dataset}.mat"
        clusters = num_clusters[dataset]
        graph, label = load_matrix(file_path)
        label = label.flatten()
        left_nodes = graph.shape[0]
        right_nodes = graph.shape[1]
        edges = []
        origin_edge = []
        for i in range(graph.shape[0]):
            for j in range(graph.shape[1]):
                if graph[i][j] > 0:
                    edges.append((i, j + left_nodes + 1))
                    origin_edge.append((i, j))

        G = nx.Graph(edges)

        # Spectral community
        partition = spectral_clustering_with_sklearn(G, clusters)
        spectral_modularity = community.modularity(partition, G)
        print(f"{dataset} :: Spectral Modularity: {spectral_modularity}")
        pre_label = np.copy(label)
        for i in range(0, left_nodes):
            pre_label[i] = partition[i]

        acc, nmi = eval(label, pre_label)

        print(f"{dataset} ACC::{acc} NIM::{nmi}\n")

        if len(edges) < 500:
            print_community(partition)
            graph_partition_visualize(G, partition, spectral_modularity)
            plt.savefig(f"./result/Spec-{dataset}-NMI.png")
            plt.cla()

        if not os.path.exists(f"./DATA/mat/{dataset}.graph"):
            save_edges(origin_edge, f"./DATA/mat/{dataset}.graph", left_nodes, right_nodes, left_nodes + right_nodes,
                       len(origin_edge))
