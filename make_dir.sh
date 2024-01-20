mkdir DATA
mkdir result
for data in "${datasets[@]}"; do
  mkdir "./DATA/${data}"
  mkdir "./result/${data}"
done

rm -r cmake-build-debug
mkdir cmake-build-debug
cd cmake-build-debug
cmake ..
make clean
make -j 40
cd ..
