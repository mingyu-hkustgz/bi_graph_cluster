cd ..
source set.sh

for data in "${datasets[@]}"; do

  graph_path="${source_path}/${data}/${data}.graph"

  result_path="data_statistic.log"
  index_path="./DATA/${data}/${data}.naive"
  ./cmake-build-debug/test/test_split_graph -g ${graph_path} -s ${result_path}&

done



