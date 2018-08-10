qemu-img create -f raw -o size=2G fs.img
rm -r ./build/*
cd build
cmake ../src
cd ..
./build.sh
