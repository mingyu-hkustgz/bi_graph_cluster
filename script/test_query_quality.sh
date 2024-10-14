cd ..
source set.sh

for data in "${datasets[@]}"; do
  graph_path="${source_path}/${data}/${data}.graph"
  cluster_path="./DATA/${data}/${data}.ivecs"
  result_path="./DATA/${data}/${data}-res.csv"
  modularity_path="./result/${data}/${data}-deg-mod.log"
  ./cmake-build-debug/test/test_degree_query_quality -g ${graph_path} -d ${cluster_path} -r ${result_path} -m ${modularity_path} &

done

for data in "${datasets[@]}"; do
  cluster_path="./DATA/${data}/${data}-proj.ivecs"
  result_path="./DATA/${data}/${data}-proj-res.csv"
  modularity_path="./result/${data}/${data}-proj-mod.log"
  index_path="./DATA/${data}/${data}.full"
  #./cmake-build-debug/test/test_proj_query_quality -g ${graph_path} -d ${cluster_path} -r ${result_path} -m ${modularity_path} &

done
