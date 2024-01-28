import os
from tqdm import tqdm
files =["edit-biwikibooks","dblp-author","citeulike-ui","dbpedia-location","munmun_twitterex_ut","amazon-ratings","flickr-groupmemberships"]


if __name__ == "__main__":
    result_path = "index_size.log"
    save_file = open(result_path,"a")
    for file in files:

        naive_path = f"./DATA/{file}/{file}.naive"
        index_path = f"./DATA/{file}/{file}.index"
        if not os.path.exists(index_path):
            continue
        index_size = os.path.getsize(index_path)
        naive_size = os.path.getsize(naive_path)
        index_size /= (1024*1024)
        naive_size /= (1024*1024)
        print(f"dataset {file} index size:: {round(index_size,6)} MB",file=save_file)
        print(f"dataset {file} naive size:: {round(naive_size,6)} MB",file=save_file)


