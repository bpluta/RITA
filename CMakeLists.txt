cmake_minimum_required(VERSION "3.12")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ./)

add_subdirectory("./zydis")
add_executable("tracer" "./backend/tracer.c")
add_library(decoder SHARED "./middleware/lib/src/decoder.c")

set_target_properties(decoder PROPERTIES LIBRARY_OUTPUT_DIRECTORY ../middleware/lib/build)
target_link_libraries("tracer" PRIVATE "Zydis")
target_link_libraries("decoder" PRIVATE "Zydis")
