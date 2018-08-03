cmake_minimum_required(VERSION 3.5)
project(os C ASM)

set(CMAKE_CROSSCOMPILING TRUE)

set(TARGET_COMPILE_TYPE i686)
set(COMPILE_TYPE x86_64)
set(COMPILER_PATH ${TARGET_COMPILE_TYPE}-elf-4.9.1-Linux-${COMPILE_TYPE})

get_filename_component(PROJECT_PATH ${PROJECT_SOURCE_DIR} DIRECTORY)

set(CMAKE_C_FLAGS "-T ${PROJECT_PATH}/linker.ld -ffreestanding -nostdlib -nostartfiles -Wall -mno-red-zone -mno-mmx -mno-sse -mno-sse2")

set(CMAKE_C_COMPILER ${PROJECT_PATH}/${COMPILER_PATH}/bin/${TARGET_COMPILE_TYPE}-elf-gcc)
set(CMAKE_ASM_COMPILER ${PROJECT_PATH}/${COMPILER_PATH}/bin/${TARGET_COMPILE_TYPE}-elf-as)

set(BUILD_NAME os)

file(GLOB_RECURSE SOURCES "../src/os/asm/${TARGET_COMPILE_TYPE}/*.S" "*.c")
file(GLOB_RECURSE HEADERS "../include/${BUILD_NAME}/*.h")

include_directories(
  ${CMAKE_CURRENT_LIST_DIR}/../include/${BUILD_NAME}
  ${CMAKE_CURRENT_LIST_DIR}/../include/${BUILD_NAME}/libc  
)

add_executable(${BUILD_NAME}.bin ${SOURCES} ${HEADERS})
