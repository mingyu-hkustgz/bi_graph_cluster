## Structural Bipartite Clustering

### C++ requirement
* boost
* OpenMP
* Please note that if you use windows or linux, please add dependencies in the corresponding Cmakelist


### python requirement
* tqdm
* matplot

### dataset
* All datasets are available at http://konect.cc/networks/

### code struct
* We provide ./script/convert_data.py to convert the original out.* files into program-readable *.graph files
*  All test programs are in the ./test/*.cpp path. Please follow the c++ longopts for adding the parameter
* If using linux, you can directly use the bash run.sh command. Please make sure that all file directories are correct.

### data file structure
* Please save the graph data in the DATA directory in the format ./DATA/{graph_file}/{graph_file}.graph

### test demo for brunson-leadership
```bash
mkdir cmake-build-debug
cd cmake-build-debug
cmake ..
make -j 16
cd ..
./cmake-build-debug/test/test_query_quality -g ./DATA/brunson_corporate-leadership_corporate-leadership/brunson_corporate-leadership_corporate-leadership.graph -i ./DATA/brunson_corporate-leadership_corporate-leadership/brunson_corporate-leadership_corporate-leadership.index -s ./DATA/brunson_corporate-leadership_corporate-leadership/ans.csv -d ./DATA/brunson_corporate-leadership_corporate-leadership/brunson_corporate-leadership_corporate-leadership.log -m 1 -l 2 -r 2 -e 0.4 -w ./DATA/brunson_corporate-leadership_corporate-leadership/brunson_corporate-leadership_corporate-leadership.weight
```

### test build efficiency
```bash
mkdir cmake-build-debug
cd cmake-build-debug
cmake ..
make -j 16
cd ..
./cmake-build-debug/test/test_index_time -g ./DATA/actor-movie/actor-movie.graph -i ./DATA/actor-movie/actor-movie.index -s ./result/actor-movie/actor-movie.log -l ./result/actor-movie/actor-movie.csv -m 1
```


### test search efficiency
```bash
mkdir cmake-build-debug
cd cmake-build-debug
cmake ..
make -j 16
cd ..
./cmake-build-debug/test/test_query_time -g ./DATA/actor-movie/actor-movie.graph -i ./DATA/actor-movie/actor-movie.index -s ./result/actor-movie/actor-movie.log -m 1
```