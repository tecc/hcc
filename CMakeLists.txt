cmake_minimum_required(VERSION 3.17)
project(hcc C)

set(CMAKE_C_STANDARD 11)

add_executable(hcc
    src/main.c

    include/hcc/lexer.h
    include/hcc/parser.h
    include/hcc/string.h
    src/lexer.c src/lexer_util.c
    src/parser.c src/parser_util.c
    src/string.c
    )
target_include_directories(hcc PRIVATE include)
target_compile_definitions(hcc PRIVATE DEBUG)