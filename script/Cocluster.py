import matplotlib
from scipy.io import loadmat
from sklearn.metrics.cluster import normalized_mutual_info_score as nmi
from coclust.coclustering import CoclustMod
from coclust.io.data_loading import load_doc_term_data
from coclust.visualization import (plot_cluster_top_terms,
                                   get_term_graph,
                                   plot_convergence)

# Retrieve the Classic3  document-term matrix from a matlab file
file_name = "./DATA/datasets/classic3.mat"
path = './DATA/datasets/classic3_coclustFormat.mat'
doc_term_data = load_doc_term_data(path)
X = doc_term_data['doc_term_matrix']
true_doc_labels = doc_term_data['doc_labels']

# Create and fit a model with 3 co-clusters
model = CoclustMod(n_clusters=3, random_state=0)
model.fit(X)

# evaluate
predicted_doc_labels = model.row_labels_
print(nmi(true_doc_labels, predicted_doc_labels))

# Plot modularities
print("MODULARITY: %s" % model.modularity)
plot_convergence(model.modularities, "Modularities")



