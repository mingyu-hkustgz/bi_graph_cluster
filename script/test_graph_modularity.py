import networkx as nx
import community
import matplotlib.pyplot as plt
import matplotlib.cm as cm


def compute_and_visualize_modularity(G):
    # Compute the best partition
    partition = community.best_partition(G)

    # Calculate modularity
    modularity = community.modularity(partition, G)

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

    return modularity, partition


def print_community(partition):
    # If you want to see the communities:
    communities = {}
    for node, community_id in partition.items():
        if community_id not in communities:
            communities[community_id] = []
        communities[community_id].append(node)

    print("\nCommunities:")
    for community_id, nodes in communities.items():
        print(f"Community {community_id}: {nodes}")


if __name__ == "__main__":
    # Example usage
    G = nx.karate_club_graph()

    modularity, partition = compute_and_visualize_modularity(G)

    print(f"Modularity: {modularity}")
    print_community(partition)
