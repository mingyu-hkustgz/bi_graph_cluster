from scipy.io import loadmat
from coclust.coclustering import CoclustMod
from coclust.coclustering import CoclustInfo
from sklearn.metrics import (adjusted_rand_score as ari,
                             normalized_mutual_info_score as nmi)

method_list = ["mod", "info"]
file_list = ["cora.mat", "WebKB_wisconsin.mat", "WebKB_texas.mat"]


if __name__ == "__main__":

    for file_name in file_list:
        file_path = f"./DATA/{file_name}"
        matlab_dict = loadmat(file_path)

        X = matlab_dict['fea']

        nb_clusters = max(matlab_dict['gnd'])[0]
        print(f"number cluster {nb_clusters} for {file_name}")
        for method in method_list:
            if method =="mod":
                model = CoclustMod(n_clusters=nb_clusters, random_state=0)
            elif method == "info":
                model = CoclustInfo(n_row_clusters=nb_clusters, n_col_clusters=nb_clusters,
                        n_init=4, random_state=0)
            
            model.fit(X)
            true_row_labels = matlab_dict['gnd'].flatten()
            predicted_row_labels = model.row_labels_
            nmi_ = nmi(true_row_labels, predicted_row_labels)
            ari_ = ari(true_row_labels, predicted_row_labels)
            print("NMI: {}\nARI: {}".format(nmi_, ari_))