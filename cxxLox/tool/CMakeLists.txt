project(cxxLox)
cmake_minimum_required(VERSION 3.16)

add_executable(cxxLoxGenrateAst
    GenerateAst.cxx
)


target_compile_options(cxxLoxGenrateAst PUBLIC
    -g
    -std=c++17
    -fsanitize=address
    -fPIC
)

target_link_directories(cxxLoxGenrateAst PUBLIC
    /usr/lib/x86_64-linux-gnu/
)

target_link_libraries(cxxLoxGenrateAst
    asan.a
    libpthread.so
    dl
    rt
    fmt.a
)

target_link_options(cxxLoxGenrateAst PRIVATE
    -fPIC
)

add_custom_command(
    COMMAND echo "start to clean generated file before...."
    COMMAND echo "rm -rf cxxLox/header/Expr.h"
    COMMAND echo "rm -rf cxxLox/header/Stmt.h"
    COMMAND rm ${LoxProjectRoot}/cxxLox/header/Expr.h ${LoxProjectRoot}/cxxLox/header/Stmt.h
    COMMAND cd ${LoxProjectRoot}/output && ./cxxLoxGenrateAst ${LoxProjectRoot}/cxxLox/header
    OUTPUT generateast
)

add_custom_target(genAstHeader
    DEPENDS cxxLoxGenrateAst
    generateast
)