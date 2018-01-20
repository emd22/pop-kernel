cmake_minimum_required(VERSION 3.5)

# set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CROSSCOMPILING TRUE)
# set(CMAKE_C_LINK_EXECUTABLE "../linker.ld")
# set(CMAKE_LINKER "../linker.ld")
set(PROJECT_PATH "/home/ethan/Desktop/os2")

set(CMAKE_C_FLAGS "-T ${PROJECT_PATH}/linker.ld -ffreestanding -nostdlib -nostartfiles")

set(CMAKE_C_COMPILER ${PROJECT_PATH}/cross-compiler/bin/i686-elf-gcc)
set(CMAKE_ASM_COMPILER ${PROJECT_PATH}/cross-compiler/bin/i686-elf-as)

project(os C ASM)

set(BUILD_NAME os)

file(GLOB_RECURSE SOURCES "*.S" "*.c")
file(GLOB_RECURSE HEADERS "../include/${BUILD_NAME}/*.h")

# foreach(f IN LISTS SOURCES)
#   get_filename_component(b ${f} NAME)
#   set_source_files_properties(${f} PROPERTIES
#     COMPILE_DEFINITIONS "__FILENAME__=${b}")
#   message(STATUS "f = ${f}")
# endforeach()

# foreach(f IN LISTS HEADERS)
#   get_filename_component(b ${f} NAME)
#   set_source_files_properties(${f} PROPERTIES
#     COMPILE_DEFINITIONS "__FILENAME__=${b}")
# endforeach()

include_directories(
  ${CMAKE_CURRENT_LIST_DIR}/../include/${BUILD_NAME}
)

add_executable(${BUILD_NAME}.bin ${SOURCES} ${HEADERS})