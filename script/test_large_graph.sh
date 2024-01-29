cd ..
source set.sh

for data in "${datasets[@]}"; do

  graph_path="${source_path}/${data}/${data}.graph"
  logger_path="./DATA/${data}/${data}-naive.log"
  result_path="./result/${data}/${data}-naive-parallel.log"
  index_path="./DATA/${data}/${data}.naive"
  ./cmake-build-debug/test/test_index_time -g ${graph_path} -i ${index_path} -l ${logger_path} -s ${result_path} -m 5 -t 80

  logger_path="./DATA/${data}/${data}-index.log"
  result_path="./result/${data}/${data}-index-parallel.log"
  index_path="./DATA/${data}/${data}.index"
  ./cmake-build-debug/test/test_index_time -g ${graph_path} -i ${index_path} -l ${logger_path} -s ${result_path} -m 7 -t 80

done



