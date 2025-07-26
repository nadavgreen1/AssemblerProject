cmake_minimum_required(VERSION 3.10)
project(AssemblerProject C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(assembler
        main.c
        globals.c
        lexer.c
        first_pass.c
        second_pass.c
        memory_image.c
        file_writer.c
        code_conversion.c
        symbol_table.c
        preproc.c
        util.c
        error_handler.c
        instructions.c
)


