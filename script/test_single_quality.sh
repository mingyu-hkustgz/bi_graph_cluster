cd ..
source set.sh

for data in "${datasets[@]}"; do
      if [ $data == "dbpedia-team" ]; then
        L=4
        R=3
        eps=0.05
      elif [ $data == "actor-movie" ]; then
        L=5
        R=3
        eps=0.05
      elif [ $data == "citeulike-ui" ]; then
        L=1
        R=1
        eps=0.05
      elif [ $data == "amazon-ratings" ]; then
        L=1
        R=1
        eps=0.05
      elif [ $data == "dblp-author" ]; then
        L=2
        R=2
        eps=0.2
      elif [ $data == "munmun_twitterex_ut" ]; then
        L=1
        R=1
        eps=0.1
      fi

  graph_path="${source_path}/${data}/${data}.graph"
  modularity_path="./result/${data}/${data}-ori-mod.log"
  ./cmake-build-debug/test/test_single_quality -g ${graph_path} -m ${modularity_path} -l ${L} -r ${R} -e ${eps} &

done

for data in "${datasets[@]}"; do
      if [ $data == "actor-movie" ]; then
        L=1
        eps=0.05
      elif [ $data == "citeulike-ui" ]; then
        L=1
        eps=0.05
      elif [ $data == "amazon-ratings" ]; then
        L=1
        eps=0.05
      elif [ $data == "dbpedia-team" ]; then
        L=1
        eps=0.05
      elif [ $data == "dblp-author" ]; then
        L=2
        eps=0.1
      elif [ $data == "munmun_twitterex_ut" ]; then
        L=2
        eps=0.05
      fi

  graph_path="${source_path}/${data}/${data}.graph"
  modularity_path="./result/${data}/${data}-deg-mod.log"
  ./cmake-build-debug/test/test_single_quality -g ${graph_path} -m ${modularity_path} -l ${L}-e ${eps} &

done
