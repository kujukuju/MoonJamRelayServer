rm -rf build/*

mkdir -p build/

cd build/

cmake -DCMAKE_CXX_COMPILER=/usr/bin/g++ -DCMAKE_C_COMPILER=/usr/bin/gcc -DCMAKE_BUILD_TYPE=Release ..

make

systemctl restart moonjam-relay
