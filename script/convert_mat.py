import os
from tqdm import tqdm
from scipy.io import loadmat
file_list = ["Yale_32x32", "cora", "WebKB4", "WebKB_wisconsin", "WebKB_texas", "IMDb_movies_actors", "IMDb_movies_keywords"]


if __name__ == "__main__":
    for dataset in file_list:
        print(f"current dataset:: {dataset}")
        file_path = f"./DATA/{dataset}.mat"
        matlab_dict = loadmat(file_path)
        X = matlab_dict['fea']

        edges = []
        left_num = 0
        right_num = 0
        for i in range(len(X)):
            for j in range(len(X[i])):
                if X[i][j] > 0:
                    edges.append((i,j))
                    left_num = max(left_num, i)
                    right_num = max(right_num, j)


        save_path = f"./DATA/{dataset}.graph"
        out_put_file = open(save_path, "w")
        node_num = left_num + right_num
        edge_num = len(edges)
        print(str(left_num) + " " + str(right_num) + " " + str(node_num) + " " + str(edge_num), file=out_put_file)
        for edges in tqdm(edges):
            print(str(edges[0]) + " " + str(edges[1]), file=out_put_file)
        
        ground = f"./DATA/{dataset}.gnd"
        out_put_file = open(ground, "w")
        true_row_labels = matlab_dict['gnd'].flatten()
        print(str(len(true_row_labels)), file=out_put_file)
        for gd in true_row_labels:
            print(str(gd) + " ", file=out_put_file)

