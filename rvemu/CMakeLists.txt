cmake_minimum_required(VERSION 3.0)

project(rvemu LANGUAGES C)

add_executable(rvemu src/rvemu.c)
target_compile_options(rvemu PRIVATE -Wall -Wextra -O3 -g -std=c18)
