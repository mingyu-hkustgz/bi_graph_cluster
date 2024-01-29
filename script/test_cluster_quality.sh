cd ..
datasets=("Yale_32x32" "cora" "WebKB4" "WebKB_wisconsin" "WebKB_texas" "IMDb_movies_actors" "IMDb_movies_keywords")
for data in "${datasets[@]}"; do

  graph_path="./DATA/${data}.graph"
  result_path="./DATA/${data}.ivecs"
  index_path="./DATA/${data}.index"
  ./cmake-build-debug/test/test_query_quality -g ${graph_path} -i ${index_path} -s ${result_path} -m 1 -l 5 -r 5 -e 0.5

done


python bench/test_cluster_quality.py

