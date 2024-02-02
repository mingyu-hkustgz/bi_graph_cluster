import numpy as np
import matplotlib.pyplot as plt

source = './DATA'


def ivecs_read(fname):
    a = np.fromfile(fname, dtype='int32')
    d = a[0]
    return a.reshape(-1, d + 1)[:, 1:].copy()


def fvecs_read(fname):
    return ivecs_read(fname).view('float32')


dataset = "./DATA/WebKB_texas.log"
if __name__ == "__main__":
    verbose = True
    print(f"visual - {dataset}")
    original_data = fvecs_read(dataset)
    original_data = original_data.flatten()
    # Samples = np.random.choice(original_data, 5000000)
    print("load finished")
    plt.figure(figsize=(5, 4))
    plt.hist(original_data, bins=100, alpha=0.8)
    plt.savefig(f"{dataset}.png")
    plt.show()
