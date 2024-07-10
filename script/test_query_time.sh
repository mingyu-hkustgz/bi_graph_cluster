cd ..
source set.sh

# for data in "${datasets[@]}"; do

#   graph_path="${source_path}/${data}/${data}.graph"

#   result_path="./result/${data}/${data}-naive-query.log"
#   index_path="./DATA/${data}/${data}.naive"
#   ./cmake-build-debug/test/test_query_time -g ${graph_path} -i ${index_path} -s ${result_path} -m 0 &

#   logger_path="./DATA/${data}/${data}-index.log"
#   result_path="./result/${data}/${data}-index-query.log"
#   index_path="./DATA/${data}/${data}.index"
#   ./cmake-build-debug/test/test_query_time -g ${graph_path} -i ${index_path} -s ${result_path} -m 1 &

<<<<<<< HEAD
# done

for data in "${datasets[@]}"; do

  graph_path="${source_path}/${data}/${data}.graph"
  logger_path="./DATA/${data}/${data}-full.log"
  result_path="./result/${data}/${data}-full-query.log"
  index_path="./DATA/${data}/${data}.full"
  ./cmake-build-debug/test/test_query_time -g ${graph_path} -i ${index_path} -s ${result_path} -m 2 &
=======
  result_path="./result/${data}/${data}-naive-query.log"
  index_path="./DATA/${data}/${data}.naive"
  ./cmake-build-debug/test/test_query_time -g ${graph_path} -i ${index_path} -s ${result_path} -m 0

  logger_path="./DATA/${data}/${data}-index.log"
  result_path="./result/${data}/${data}-index-query.log"
  index_path="./DATA/${data}/${data}.index"
  ./cmake-build-debug/test/test_query_time -g ${graph_path} -i ${index_path} -s ${result_path} -m 1
>>>>>>> 23ac9f5a7c8b3cd5fece34b5481e64b425d114cc

done
