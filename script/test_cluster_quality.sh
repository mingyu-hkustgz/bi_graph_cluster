cd ..

eps=0.5
left=5
right=5
datasets=("Yale_32x32" "cora" "WebKB4" "WebKB_wisconsin" "WebKB_texas" "IMDb_movies_actors" "IMDb_movies_keywords")
for data in "${datasets[@]}"; do

  graph_path="./DATA/${data}.graph"
  result_path="./DATA/${data}.ivecs"
  index_path="./DATA/${data}.index"

  ./cmake-build-debug/test/test_query_quality -g ${graph_path} -i ${index_path} -s ${result_path} -m 1 -l $left -r $right -e $eps

done


python bench/test_cluster_quality.py

