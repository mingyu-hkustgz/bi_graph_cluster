cd ..
source set.sh



for data in "${datasets[@]}"; do
  graph_path="${source_path}/${data}/${data}.graph"
  logger_path="./DATA/${data}/${data}.log"
  result_path="./result/${data}/${data}-naive-build.log"
  weight_path="./DATA/${data}/${data}.weight"
  cluster_path="./DATA/${data}/${data}.ivecs"
  ./cmake-build-debug/test/test_query -g ${graph_path} -i ${index_path} -d ${cluster_path} -w ${weight_path} -l ${logger_path} -s ${result_path} -m 0

done


