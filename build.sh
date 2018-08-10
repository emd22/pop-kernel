cd build
make || exit 1
cd ..
./genisoimg.sh
#VBoxManage startvm "os"
qemu-system-i386 -cdrom ./iso.iso -drive id=disk,if=none,file=fs.img,format=raw -device ahci
