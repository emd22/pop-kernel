cd build
make || exit 1
cd ..
./genisoimg.sh
#VBoxManage startvm "os"
qemu-system-i386 -drive file=fs.img,format=raw -cdrom ./os.iso
