cd ..
source set.sh



for data in "${datasets[@]}"; do
  graph_path="${source_path}/${data}/${data}.graph"
  index_path="./DATA/${data}/${data}.index"
  ./cmake-build-debug/test/test_dynamic_index -g ${graph_path} -i ${index_path} -t 10

  graph_path="${source_path}/${data}/${data}.graph"
  index_path="./DATA/${data}/${data}.index"
  ./cmake-build-debug/test/test_dynamic_index -g ${graph_path} -i ${index_path} -t 20

  graph_path="${source_path}/${data}/${data}.graph"
  index_path="./DATA/${data}/${data}.index"
  ./cmake-build-debug/test/test_dynamic_index -g ${graph_path} -i ${index_path} -t 30

    graph_path="${source_path}/${data}/${data}.graph"
    index_path="./DATA/${data}/${data}.index"
    ./cmake-build-debug/test/test_dynamic_index -g ${graph_path} -i ${index_path} -t 300

    graph_path="${source_path}/${data}/${data}.graph"
    index_path="./DATA/${data}/${data}.index"
    ./cmake-build-debug/test/test_dynamic_index -g ${graph_path} -i ${index_path} -t 300

done


