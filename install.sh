printf "Checking Dependencies... "
#this fixes the weird grub-mkrescue fail issue
if [ command -v grub-pc-bin &> /dev/null ]; then
    sudo apt install grub-pc-bin
fi

echo Done.
echo Checking cross-compiler...

if [ ! -d x86_64-elf-4.9.1-Linux-x86_64 ] || [ ! -d i686-elf-4.9.1-Linux-x86_64 ]; then
    # if [ -d x86_64-elf-4.9.1-Linux-x86_64 ]; then
    #     mv x86_64-elf-4.9.1-Linux-x86_64 cross-compiler
    # elif [ -d i686-elf-4.9.1-Linux-x86_64 ]; then
    #     mv i686-elf-4.9.1-Linux-x86_64 cross-compiler
    # else
    echo No cross-compiler found. Download cross compiler from proper link below and rerun this script.
    echo x86_64: http://newos.org/toolchains/x86_64-elf-4.9.1-Linux-x86_64.tar.xz
    echo x86: http://newos.org/toolchains/i686-elf-4.9.1-Linux-x86_64.tar.xz
    exit 1
    # fi
fi

mkdir -p build
cd build
cmake ../src
make
mkdir -p out
cd out