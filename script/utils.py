import numpy as np


def ivecs_read(fname):
    a = np.fromfile(fname, dtype='int32')
    d = a[0]
    return a.reshape(-1, d + 1)[:, 1:].copy()


def fvecs_read(fname):
    return ivecs_read(fname).view('float32')


def ivecs_mmap(fname):
    a = np.memmap(fname, dtype='int32', mode='r')
    d = a[0]
    return a.reshape(-1, d + 1)[:, 1:]


def fvecs_mmap(fname):
    return ivecs_mmap(fname).view('float32')


def ivecs_write(fname, m):
    n, d = m.shape
    m1 = np.empty((n, d + 1), dtype='int32')
    m1[:, 0] = d
    m1[:, 1:] = m
    m1.tofile(fname)


def fvecs_write(fname, m):
    m = m.astype('float32')
    ivecs_write(fname, m.view('int32'))


def load_bipartite_graph(fname):
    f = open(fname, "r")
    line = f.readline()
    raw = line.split()
    left_num = int(raw[0])
    right_num = int(raw[1])
    node_num = int(raw[2])
    edge_num = int(raw[3])
    edges = []
    count = 0
    while line:
        line = f.readline()
        if line == "":
            break
        raw = line.split()
        left_node = int(raw[0])
        right_node = int(raw[1]) + left_num + 1
        edges.append((left_node, right_node))
        count += 1
        if count % 1000000 == 0:
            print("current tag:: " + str(count) + " left id:: " + str(left_num))
    f.close()
    return node_num, edge_num, edges

def load_graph_partition_from_csv(fname):
    f = open(fname, "r")
    line = f.readline()
    partition = {}
    while line:
        line = f.readline()
        if line == "":
            break
        raw = line.split(",")
        node = int(raw[0])
        belong = int(raw[1])
        partition[node] = belong
    f.close()
    return partition
