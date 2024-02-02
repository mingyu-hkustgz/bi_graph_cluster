cd ..
source set.sh


for data in "${datasets[@]}"; do
  graph_path="${source_path}/${data}/${data}.graph"
  logger_path="./DATA/${data}/${data}.log"
  result_path="./result/${data}/${data}-naive-base-build.log"
  index_path="./DATA/${data}/${data}.naive"
  ./cmake-build-debug/test/test_index_without_join -g ${graph_path} -i ${index_path} -l ${logger_path} -s ${result_path} -m 0 &

  result_path="./result/${data}/${data}-index-base-build.log"
  index_path="./DATA/${data}/${data}.index"
  ./cmake-build-debug/test/test_index_without_join -g ${graph_path} -i ${index_path} -l ${logger_path} -s ${result_path} -m 1 &
done


for data in "${datasets[@]}"; do
  graph_path="${source_path}/${data}/${data}.graph"
  logger_path="./DATA/${data}/${data}.log"
  result_path="./result/${data}/${data}-naive-fast-build.log"
  index_path="./DATA/${data}/${data}.naive"
  ./cmake-build-debug/test/test_index_time -g ${graph_path} -i ${index_path} -l ${logger_path} -s ${result_path} -m 0 &

  result_path="./result/${data}/${data}-index-fast-build.log"
  index_path="./DATA/${data}/${data}.index"
  ./cmake-build-debug/test/test_index_time -g ${graph_path} -i ${index_path} -l ${logger_path} -s ${result_path} -m 1 &
done

wait

for data in "${datasets[@]}"; do

  graph_path="${source_path}/${data}/${data}.graph"
  logger_path="./DATA/${data}/${data}-naive.log"
  result_path="./result/${data}/${data}-naive-parallel.log"
  index_path="./DATA/${data}/${data}.naive"
  ./cmake-build-debug/test/test_index_time -g ${graph_path} -i ${index_path} -l ${logger_path} -s ${result_path} -m 2 -t 16

  logger_path="./DATA/${data}/${data}-index.log"
  result_path="./result/${data}/${data}-index-parallel.log"
  index_path="./DATA/${data}/${data}.index"
  ./cmake-build-debug/test/test_index_time -g ${graph_path} -i ${index_path} -l ${logger_path} -s ${result_path} -m 3 -t 16

done



