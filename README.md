# Pop Kernel
Pop kernel is a x86 kernel written almost completely in C. Some of the current features include:
- PS2 keyboard
- MBR reading/writing
- WIP AHCI driver
- WIP PCI driver
- WIP SATA driver

The main goal for Pop Kernel is to be fast and able to run on low-end computers. The kernel will be built using "modules" of lua files. Many drivers will be downloadable and would be run as a process. For example, the UI would be some lua files which could be edited/worked on while the operating system is running. This would be done by having a reserved ram disk that will hold all system files. if the operating system finds the refresh command, it will test to see if the modules are runnable. I chose to use lua because of its speed, portability and size.

## Getting started
To get started with Pop Kernel, you will have to clone the repository. This could easily be done by the```git clone``` command. After cloning, you will need to edit the CMakeLists.txt file. For the ```PROJECT_PATH``` variable, insert the full path to the cloned source. You can install any prerequisites by running ```install.sh```. you will need the cross-compiler, which you can find the link by running the install script.
### Building
To build, run the following commands:
```
mkdir build
cd build
cmake ../src
```
To finish the build process, run ```make```. For ease of use, you can run ```build.sh```. You can comment out the line for starting virtualbox and manually do it, or download the extensions package off their website.