import os
from tqdm import tqdm

source = '/home/BLD/mingyu/DATA/bi-graph_data'
if __name__ == "__main__":
    # files = os.listdir(source)
    # print(files)
    # files.sort()
    files =["edit-biwikibooks", "citeulike-ui", "dblp-author","flickr-groupmemberships"]
    for dataset in files:
        data_file_path = f"{source}/{dataset}/out.{dataset}"
        print(f"now:: data set -> {dataset}")
        f = open(data_file_path, "r")
        line = f.readline()
        node_num = 0
        edge_num = 0
        left_num = 0
        right_num = 0
        edges = []
        count = 0
        while line:
            line = line.replace('\t', ' ')
            raw = line.split()
            line = f.readline()
            if raw[0] == '%':
                continue
            edges.append((int(raw[0]), int(raw[1])))
            left_num = max(left_num, int(raw[0]))
            right_num = max(right_num, int(raw[1]))
            count += 1
            if count % 1000000 == 0:
                print("current tag:: " + str(count) + " left id:: " + str(left_num))
            if line == "":
                break
        f.close()

        save_path = f"{source}/{dataset}/{dataset}.graph"
        out_put_file = open(save_path, "w")
        node_num = left_num + right_num
        edge_num = len(edges)
        print(str(left_num) + " " + str(right_num) + " " + str(node_num) + " " + str(edge_num), file=out_put_file)
        for edges in tqdm(edges):
            print(str(edges[0]) + " " + str(edges[1]), file=out_put_file)
