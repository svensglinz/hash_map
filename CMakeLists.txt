cmake_minimum_required(VERSION 3.29)
project(hash_map C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(hash_map
    hash_map.c
    hash_map.h
    map_test.c)
