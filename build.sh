cd build
make || exit 1
cd ..
./genisoimg.sh
#VBoxManage startvm "os"
qemu-system-i386 -cdrom ./os.iso -drive file=fs.img,format=raw
