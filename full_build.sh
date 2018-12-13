qemu-img create -f raw -o size=32M fs.img
rm -r ./build/*
cd build
cmake ../src
cd ..
./build.sh
