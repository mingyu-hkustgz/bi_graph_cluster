cd ..
source set.sh

for data in "${datasets[@]}"; do

  graph_path="${source_path}/${data}/${data}.graph"

  result_path="./result/${data}/${data}-naive-query.log"
  index_path="./DATA/${data}/${data}.naive"
  ./cmake-build-debug/test/test_query_time -g ${graph_path} -i ${index_path} -s ${result_path} -m 0

  logger_path="./DATA/${data}/${data}-index.log"
  result_path="./result/${data}/${data}-index-query.log"
  index_path="./DATA/${data}/${data}.index"
  ./cmake-build-debug/test/test_query_time -g ${graph_path} -i ${index_path} -s ${result_path} -m 1

done
