cmake_minimum_required(VERSION 3.5)

project(os)

set(BUILD_NAME os)
set(CMAKE_C_COMPILER ../cross-compiler/bin/i686-elf-gcc)
set(CMAKE_ASM_COMPILER ../cross-compiler/bin/i686-elf-as)

set(LINKER_SCRIPT "../linker.ld")
set(CMAKE_C_FLAGS "-ffreestanding -nostdlib")

ENABLE_LANGUAGE(ASM-ATT)

file(GLOB_RECURSE SOURCES "*.c")
file(GLOB_RECURSE HEADERS "../include/${BUILD_NAME}/*.h")
file(GLOB_RECURSE ASM_SRCS "*.S")

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

include_directories(
  ${CMAKE_CURRENT_LIST_DIR}/../include/${BUILD_NAME}
)

add_executable(${BUILD_NAME}.bin ${ASM_SRCS} ${SOURCES} ${HEADERS})