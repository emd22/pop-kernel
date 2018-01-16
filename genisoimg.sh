# mkisofs -R -b os.bin -no-emul-boot -boot-load-size 4 -boot-info-table -o os.iso ./build/out build
mkdir -p isodir/boot/grub
cp build/os.bin isodir/boot/os.bin
cp grub.cfg isodir/boot/grub/grub.cfg
grub-mkrescue -o os.iso isodir