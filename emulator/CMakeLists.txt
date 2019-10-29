cmake_minimum_required(VERSION 3.0)

project(emulator LANGUAGES C)

add_executable(emulator src/main.c)
target_compile_options(emulator PRIVATE -Wall -Wextra -O3 -g -std=c18)
