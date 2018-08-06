cd build
make || exit 1
cd ..
./genisoimg.sh
VBoxManage startvm "os"