echo Installing Dependencies... 
sudo apt install gcc
#this fixes the weird grub-mkrescue fail issue
sudo apt install grub-pc-bin
echo Checking cross-compiler

if [ ! -d cross-compiler ]; then
    if [ -d i686-elf-4.9.1-Linux-x86_64 ]; then
        mv i686-elf-4.9.1-Linux-x86_64 cross-compiler
    else
        echo No cross-compiler found. Download from http://newos.org/toolchains/i686-elf-4.9.1-Linux-x86_64.tar.xz and rerun this script.
        exit 1
    fi
fi

mkdir -p build
cd build
cmake ../src
make
mkdir -p out
cd out