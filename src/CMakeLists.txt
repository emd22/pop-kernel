cmake_minimum_required(VERSION 3.5)
project(os C ASM)

set(CMAKE_CROSSCOMPILING TRUE)
set(COMPILE_TYPE i686)

#FIXME
#CHANGE THIS      vvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
set(PROJECT_PATH "/home/ethan/Desktop/pop-kernel")
#                 ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

set(CMAKE_C_FLAGS "-T ${PROJECT_PATH}/linker.ld -ffreestanding -nostdlib -nostartfiles -Wall")

set(CMAKE_C_COMPILER ${PROJECT_PATH}/cross-compiler/bin/${COMPILE_TYPE}-elf-gcc)
set(CMAKE_ASM_COMPILER ${PROJECT_PATH}/cross-compiler/bin/${COMPILE_TYPE}-elf-as)

set(BUILD_NAME os)

file(GLOB_RECURSE SOURCES "*.S" "*.c")
file(GLOB_RECURSE HEADERS "../include/${BUILD_NAME}/*.h")

include_directories(
  ${CMAKE_CURRENT_LIST_DIR}/../include/${BUILD_NAME}
)

add_executable(${BUILD_NAME}.bin ${SOURCES} ${HEADERS})
