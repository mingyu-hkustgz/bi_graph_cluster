cd ..
source set.sh

for data in "${datasets[@]}"; do
      if [ $data == "leader" ]; then
        L=2
        R=2
        eps=0.4
      elif [ $data == "revolution" ]; then
        L=1
        R=1
        eps=0.2
      elif [ $data == "dbpedia-writer" ]; then
        L=1
        R=1
        eps=0.1
      elif [ $data == "actor-movie" ]; then
        L=5
        R=3
        eps=0.05
      elif [ $data == "citeulike-ui" ]; then
        L=1
        R=1
        eps=0.05
      elif [ $data == "github" ]; then
        L=2
        R=1
        eps=0.15
      elif [ $data == "amazon-ratings" ]; then
        L=1
        R=1
        eps=0.05
      elif [ $data == "flickr-groupmemberships" ]; then
        L=20
        R=80
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
