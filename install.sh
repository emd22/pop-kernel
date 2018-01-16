echo Installing Dependencies...
sudo apt install bchunk
sudo apt install gcc
echo Checking cross-compiler

if [ ! -d cross-compiler ]; then
    if [ -d i686-elf-4.9.1-Linux-x86_64 ]; then
        mv i686-elf-4.9.1-Linux-x86_64 cross-compiler
    else
        echo No cross-compiler found. Download from http://newos.org/toolchains/i686-elf-4.9.1-Linux-x86_64.tar.xz.
        exit 1
    fi
fi

mkdir -p build
cd build
cmake ../src
make
mkdir -p out
cd out