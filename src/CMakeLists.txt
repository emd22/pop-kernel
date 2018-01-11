project(os)

set(BUILD_NAME os)
set(CMAKE_C_COMPILER ../i686-elf-4.9.1-Linux-x86_64/bin/i686-elf-gcc)
set(COMPILE_FLAGS "-T linker.ld -ffreestanding -nostdlib -nostartfiles -o ${BUILD_NAME}.bin -w")

cmake_minimum_required(VERSION 3.5)

file (GLOB_RECURSE SOURCES "*.c")
file (GLOB_RECURSE HEADERS "../include/${BUILD_NAME}/*.h")

foreach(f IN LISTS SOURCES)
  get_filename_component(b ${f} NAME)
  set_source_files_properties(${f} PROPERTIES
    COMPILE_DEFINITIONS "__FILENAME__=${b}")
endforeach()
foreach(f IN LISTS HEADERS)
  get_filename_component(b ${f} NAME)
  set_source_files_properties(${f} PROPERTIES
    COMPILE_DEFINITIONS "__FILENAME__=${b}")
endforeach()

add_definitions(${COMPILE_FLAGS})

include_directories(
  ${CMAKE_CURRENT_LIST_DIR}/../include/${BUILD_NAME}
)

add_executable(${BUILD_NAME} ${SOURCES} ${HEADERS})