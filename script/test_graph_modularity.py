import networkx as nx
import community
import matplotlib.pyplot as plt
import matplotlib.cm as cm
from sklearn.cluster import KMeans
import scipy.io as scio
from sklearn.cluster import SpectralClustering
from sklearn.cluster import SpectralCoclustering
from sklearn.cluster import SpectralBiclustering
from coclust.coclustering import CoclustMod
from scipy.sparse import csr_matrix
from utils import *
from time import *

# datasets = ["dbpedia-writer", "actor-movie", "citeulike-ui", "github", "leader", "revolution"]

datasets = ["revolution", "leader"]
num_cluster = 5
Methods = ["index", "Spec" , "Mod"]


def spectral_clustering_with_sklearn(G, n_clusters):
    # obtain adj-matrix
    adj_matrix = nx.to_numpy_array(G)

    # SpectralClustering
    sc = SpectralClustering(n_clusters=n_clusters, affinity='precomputed', n_init=100, assign_labels='discretize')
    sc.fit(adj_matrix)

    # obtain cluster result
    clustering = {node: label for node, label in zip(G.nodes(), sc.labels_)}

    return clustering



def Coluster_MOD(mat, n_clusters):
    model = CoclustMod(n_clusters=n_clusters)
    model.fit(mat)
    print(model.modularity, "check MOD")
    # obtain cluster result
    clustering = {}
    count = 1

    for label in model.row_labels_:
        clustering[count] = label
        count += 1

    count += 1
    for label in model.column_labels_:
        clustering[count] = label
        count += 1

    return clustering


def graph_partition_visualize(G, partition, modularity):
    # Draw the graph
    pos = nx.spring_layout(G)

    # Color the nodes according to their partition
    cmap = cm.get_cmap('viridis', max(partition.values()) + 1)
    nx.draw_networkx_nodes(G, pos, partition.keys(), node_size=40,
                           cmap=cmap, node_color=list(partition.values()))
    nx.draw_networkx_edges(G, pos, alpha=0.5)

    plt.title(f"Graph Partition (Modularity: {modularity:.4f})")
    plt.axis('off')
    plt.tight_layout()
    plt.show()


def print_community(partition):
    # If you want to see the communities:
    communities = {}
    for node, community_id in partition.items():
        if community_id not in communities:
            communities[community_id] = []
        communities[community_id].append(node)

    print("Communities:")
    for community_id, nodes in communities.items():
        print(f"Community {community_id}: {nodes}")


def load_index_community(fname):
    community_data = ivecs_read(fname)
    index_community = {}
    for i in range(community_data.shape[0]):
        index_community[community_data[i][0]] = community_data[i][1]

    return index_community


if __name__ == "__main__":
    for dataset in datasets:

        for method in ["origin"]:
            print(f"Method -: {method}")
            if method == "origin":
                # Example usage
                node_num, edge_num, edges = load_bipartite_graph(f"./DATA/{dataset}/{dataset}.graph")

                # G = nx.karate_club_graph()
                G = nx.Graph(edges)
            else:
                # Example usage
                node_num, edge_num, edges = load_bipartite_graph(f"./DATA/{dataset}/{dataset}.graph")
                graph_filter = load_graph_partition_from_csv(f"./DATA/{dataset}/{dataset}-res.csv")

                new_edges = []
                for edge in edges:
                    if graph_filter[edge[0]] == -1 or graph_filter[edge[1]] == -1:
                        continue
                    new_edges.append(edge)
                G = nx.Graph(new_edges)
            for cluster_method in Methods:
                if cluster_method == "index":
                    ################################ Index Method #################################
                    # Index community
                    if method == "origin":
                        index_community = load_index_community(f"./DATA/{dataset}/{dataset}.ivecs")
                    else:
                        index_community = {}
                        for node, cluster in graph_filter.items():
                            if node not in G.nodes:
                                continue
                            if cluster != -1:
                                index_community[node] = cluster

                    index_modularity = community.modularity(index_community, G)

                    print(f"\nIndex Modularity: {index_modularity}\n")
                    if len(edges) < 1000:
                        print_community(index_community)
                        graph_partition_visualize(G, index_community, index_modularity)
                        plt.savefig(f"./result/Index-{dataset}-{cluster_method}.png")
                        plt.cla()

                if cluster_method == "Spec":
                    ################################ Spectral Method #################################
                    t1 = time()
                    # Spectral community
                    partition = spectral_clustering_with_sklearn(G, num_cluster)
                    t2 = time()
                    spectral_modularity = community.modularity(partition, G)

                    print(f"\nSpectral Modularity: {spectral_modularity}\n")
                    print(f"Spect Time Use {t2-t2}(s)\n")
                    if len(edges) < 1000:
                        print_community(partition)
                        graph_partition_visualize(G, partition, spectral_modularity)
                        plt.savefig(f"./result/Spec-{dataset}-{cluster_method}.png")
                        plt.cla()

                if cluster_method == "Mod":
                    ################################ Mod Method #################################

                    # Mod Co-cluster community
                    t1 = time()
                    mat = load_bipartite_sparse(f"./DATA/{dataset}/{dataset}.graph")
                    partition = Coluster_MOD(mat, num_cluster)
                    t2 = time()

                    MoD_modularity = community.modularity(partition, G)

                    print(f"\nMOD Modularity: {MoD_modularity}\n")
                    print(f"Mod Time Use {t2-t2}(s)\n")
                    if len(edges) < 1000:
                        print_community(partition)
                        graph_partition_visualize(G, partition, MoD_modularity)
                        plt.savefig(f"./result/Co-MoD-{dataset}-{cluster_method}.png")
                        plt.cla()

                if cluster_method == "Kmeans":
                    ################################ Kmeans Method #################################
                    mat = nx.to_numpy_array(G)
                    kmeans = KMeans(n_clusters=num_cluster, random_state=42)
                    kmeans.fit(mat)
                    print(kmeans.labels_)
                    partition = {node: label for node, label in zip(G.nodes(), kmeans.labels_)}

                    MoD_modularity = community.modularity(partition, G)

                    print(f"\nKeamns Modularity: {MoD_modularity}\n")
                    if len(edges) < 1000:
                        print_community(partition)
                        graph_partition_visualize(G, partition, MoD_modularity)
                        plt.savefig(f"./result/kmeans-{dataset}-{cluster_method}.png")
                        plt.cla()
