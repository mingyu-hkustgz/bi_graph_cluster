import os
from tqdm import tqdm

source_path = './DATA'
if __name__ == "__main__":
    files = os.listdir(source_path)
    print(files)
    files.sort()
    for data in files:
        print(f"now:: data set -> {data}")
        graph_path = f"{source_path}/{data}/{data}.graph"
        logger_path = f"./DATA/{data}/{data}.log"
        result_path = f"./result/{data}/{data}-naive-base-build.log"
        index_path = f"./DATA/{data}/{data}.naive"

        os.system(f"mkdir ./result/{data}")
        os.system(
            f"start ./cmake-build-debug/test/test_index_without_join.exe -g {graph_path} -i {index_path} -l {logger_path} -s {result_path} -m 0")

        result_path = f"./result/{data}/{data}-index-base-build.log"
        index_path = f"./DATA/{data}/{data}.index"
        os.system(
            f"start  ./cmake-build-debug/test/test_index_without_join.exe -g {graph_path} -i {index_path} -l {logger_path} -s {result_path} -m 1")

        graph_path = f"{source_path}/{data}/{data}.graph"
        logger_path = f"./DATA/{data}/{data}.log"
        result_path = f"./result/{data}/{data}-naive-fast-build.log"
        index_path = f"./DATA/{data}/{data}.naive"
        os.system(
            f"start ./cmake-build-debug/test/test_index_time.exe -g {graph_path} -i {index_path} -l {logger_path} -s {result_path} -m 0")

        result_path = f"./result/{data}/{data}-index-fast-build.log"
        index_path = f"./DATA/{data}/{data}.index"
        os.system(
            f"start  ./cmake-build-debug/test/test_index_time.exe -g {graph_path} -i {index_path} -l {logger_path} -s {result_path} -m 1")

        graph_path = f"{source_path}/{data}/{data}.graph"
        logger_path = f"./DATA/{data}/{data}-naive.log"
        result_path = f"./result/{data}/{data}-naive-parallel.log"
        index_path = f"./DATA/{data}/{data}.naive"
        os.system(
            f"start ./cmake-build-debug/test/test_index_time.exe -g {graph_path} -i {index_path} -l {logger_path} -s {result_path} -m 2")

        graph_path = f"{source_path}/{data}/{data}.graph"
        logger_path = f"./DATA/{data}/{data}-index.log"
        result_path = f"./result/{data}/{data}-index-parallel.log"
        index_path = f"./DATA/{data}/{data}.index"
        os.system(
            f"start "
            f"./cmake-build-debug/test/test_index_time.exe -g {graph_path} -i {index_path} -l {logger_path} -s {result_path} -m 2")
