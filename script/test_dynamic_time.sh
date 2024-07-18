cd ..
source set.sh



for data in "${datasets[@]}"; do
  graph_path="${source_path}/${data}/${data}.graph"
  index_path="./DATA/${data}/${data}.index"
  result_path="./result/${data}/${data}-dynamic-time.log"

  ./cmake-build-debug/test/test_dynamic_time -g ${graph_path} -i ${index_path} -r ${result_path} -m 0 -t 1000 &

  ./cmake-build-debug/test/test_dynamic_time -g ${graph_path} -i ${index_path} -r ${result_path} -m 1 -t 1000 &

  ./cmake-build-debug/test/test_dynamic_time -g ${graph_path} -i ${index_path} -r ${result_path} -m 2 -t 1000 &

  ./cmake-build-debug/test/test_dynamic_time -g ${graph_path} -i ${index_path} -r ${result_path} -m 3 -t 1000 &


done


