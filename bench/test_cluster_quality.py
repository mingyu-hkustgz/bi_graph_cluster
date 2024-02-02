from scipy.io import loadmat
import numpy as np
from sklearn.metrics import (adjusted_rand_score as ari,
                             normalized_mutual_info_score as nmi)


file_list = ["cora.mat","WebKB_wisconsin.mat", "WebKB_texas.mat"]

def ivecs_read(fname):
    a = np.fromfile(fname, dtype='int32')
    d = a[0]
    return a.reshape(-1, d + 1)[:, 1:].copy()


if __name__ == "__main__":

    for file_name in file_list:
        file_path = f"./DATA/{file_name}/{file_name}.ivecs"
        ground_path = f"./DATA/{file_name}/{file_name}_groundtruth.ivecs"
        predict_ans = ivecs_read(file_path)
        predict_ans = predict_ans.flatten()
        true_row_labels = ivecs_read(ground_path)
        print(predict_ans)
        print(max(predict_ans))
        print(true_row_labels)
        print(f"currnet file:: {file_name}")
        nmi_ = nmi(true_row_labels, predict_ans)
        ari_ = ari(true_row_labels, predict_ans)
        print("NMI: {}\nARI: {}".format(nmi_, ari_))

