project(cxxLox)
cmake_minimum_required(VERSION 3.16)

add_executable(AstPrinter
    AstPrinter.cxx
    ${CMAKE_SOURCE_DIR}/cxxLox/src/any_type.cxx
    ${CMAKE_SOURCE_DIR}/cxxLox/src/Token.cxx
)

add_dependencies(AstPrinter genAstHeader)

target_include_directories(AstPrinter PUBLIC
    ${CMAKE_SOURCE_DIR}/cxxLox/header/
    ${CMAKE_SOURCE_DIR}/cxxLox/utils/
)


target_compile_options(AstPrinter PUBLIC
    -g
    -std=c++17
    -fsanitize=address
)

target_link_directories(AstPrinter PUBLIC
    /usr/lib/x86_64-linux-gnu/
)

target_link_libraries(AstPrinter
    asan.a
    libpthread.so
    dl
    rt
    fmt.a
)

add_custom_command(
    COMMAND echo "start to print AST:"

    COMMAND cd ${LoxProjectRoot}/output && ./AstPrinter
    OUTPUT AstPrinter
)

add_custom_target(test
    DEPENDS AstPrinter genAstHeader
    AstPrinter
)