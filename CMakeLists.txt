cmake_minimum_required(VERSION 3.16)
project(zlib1 C)

set(CMAKE_C_STANDARD 99)

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_STATIC_LIBRARY_PREFIX "")

# https://stackoverflow.com/questions/18138635/mingw-exe-requires-a-few-gcc-dlls-regardless-of-the-code
add_link_options(-static -static-libgcc -static-libstdc++)

add_library(zlib1 SHARED proxy.h library.def hook.c)
link_libraries(ws2_32)

add_subdirectory(minhook)
target_compile_options(minhook PUBLIC -w)
target_link_libraries(zlib1 minhook)
