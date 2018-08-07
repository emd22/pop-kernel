#DO NOT DO THIS FOR EVERY BUILD(rebuilds cmake files, so it's slow)

rm -r ./build/*
cd build
cmake ../src
cd ..
./build.sh