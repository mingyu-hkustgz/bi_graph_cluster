import networkx as nx
import community
import matplotlib.pyplot as plt
import matplotlib.cm as cm
from networkx.algorithms import community as community_algorithm
from sklearn.cluster import SpectralClustering
from utils import *

# datasets = ["dbpedia-writer", "actor-movie", "citeulike-ui", "github", "leader", "revolution"]

datasets = ["dbpedia-writer"]


def compute_modularity(G):
    # Compute the best partition
    partition = community.best_partition(G)

    # Calculate modularity
    modularity = community.modularity(partition, G)

    return modularity, partition


def spectral_clustering_with_sklearn(G, n_clusters):
    # obtain adj-matrix
    adj_matrix = nx.to_numpy_array(G)

    # SpectralClustering
    sc = SpectralClustering(n_clusters=n_clusters, affinity='precomputed', n_init=100, assign_labels='discretize')
    sc.fit(adj_matrix)

    # obtain cluster result
    clustering = {node: label for node, label in zip(G.nodes(), sc.labels_)}

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

        for method in ["origin", "split"]:
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

            # Index community
            if method == "origin":
                index_community = load_index_community(f"./DATA/{dataset}/{dataset}.ivecs")
            else:
                index_community = {}
                for node, cluster in graph_filter.items():
                    if cluster != -1:
                        index_community[node] = cluster

            index_modularity = community.modularity(index_community, G)

            print(f"\nIndex Modularity: {index_modularity}\n")
            if len(edges) < 1000:
                print_community(index_community)
                graph_partition_visualize(G, index_community, index_modularity)
                plt.savefig(f"./result/Index-{dataset}-{method}.png")
                plt.cla()

            # MAX community

            modularity, partition = compute_modularity(G)

            print(f"\nMAX Modularity: {modularity}\n")
            if len(edges) < 1000:
                print_community(partition)
                graph_partition_visualize(G, partition, modularity)
                plt.savefig(f"./result/Max-{dataset}-{method}.png")
                plt.cla()

            # Spectral community

            continue

            partition = spectral_clustering_with_sklearn(G, 5)

            spectral_modularity = community.modularity(partition, G)

            print(f"\nSpectral Modularity: {spectral_modularity}\n")
            if len(edges) < 1000:
                print_community(partition)
                graph_partition_visualize(G, partition, spectral_modularity)
                plt.savefig(f"./result/Spec-{dataset}-{method}.png")
                plt.cla()
