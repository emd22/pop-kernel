cd build
make || exit 1
cd ..
./genisoimg.sh
#-boot d tells qemu to boot from cdrom and not drive
qemu-system-i386 -boot d -cdrom ./os.iso -drive file=fs.img,format=raw 
