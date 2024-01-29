from scipy.io import loadmat
import numpy as np
from sklearn.metrics import (adjusted_rand_score as ari,
                             normalized_mutual_info_score as nmi)


file_list = ["Yale_32x32.mat", "cora.mat", "WebKB4.mat", "WebKB_wisconsin.mat", "WebKB_texas.mat", "IMDb_movies_actors.mat", "IMDb_movies_keywords.mat"]

def ivecs_read(fname):
    a = np.fromfile(fname, dtype='int32')
    d = a[0]
    return a.reshape(-1, d + 1)[:, 1:].copy()


if __name__ == "__main__":

    for file_name in file_list:
        file_path = f"./DATA/{file_name}"
        ans = file_name.replace("mat","ivecs")
        ans_path = f"./DATA/{ans}"
        predict_ans = ivecs_read(ans_path)
        matlab_dict = loadmat(file_path)

        predict_ans = predict_ans.flatten()
        true_row_labels = matlab_dict['gnd'].flatten()
        print(f"currnet file:: {file_name}")
        nmi_ = nmi(true_row_labels, predict_ans)
        ari_ = ari(true_row_labels, predict_ans)
        print("NMI: {}\nARI: {}".format(nmi_, ari_))

