cd ..

eps=0.5
left=5
right=5
datasets=("Yale_32x32" "cora" "WebKB4" "WebKB_wisconsin" "WebKB_texas" "IMDb_movies_actors" "IMDb_movies_keywords")
for data in "${datasets[@]}"; do

  graph_path="./DATA/${data}.graph"
  result_path="./DATA/${data}.ivecs"
  index_path="./DATA/${data}.index"

    if [ $data == "Yale_32x32" ]; then
      eps=-1
      left=10
      right=10
    elif [ $data == "core" ]; then
      eps=-1
      left=10
      right=10
    elif [ $data == "WebKB4" ]; then
      eps=-1
      left=5
      right=5
    elif [ $data == "WebKB_wisconsin" ]; then
      eps=-1
      left=5
      right=5
    elif [ $data == "WebKB_texas" ]; then
      eps=-1
      left=2
      right=6
    elif [ $data == "IMDb_movies_actors" ]; then
      eps=0.5
      left=5
      right=5
    elif [ $data == "IMDb_movies_keywords" ]; then
      eps=0.5
      left=5
      right=5
    fi

  ./cmake-build-debug/test/test_query_quality -g ${graph_path} -i ${index_path} -s ${result_path} -m 1 -l $left -r $right -e $eps

done


python bench/test_cluster_quality.py

