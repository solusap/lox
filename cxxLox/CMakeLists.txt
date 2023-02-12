project(cxxLox)
cmake_minimum_required(VERSION 3.16)


message(STATUS "CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}")

set(LoxProjectRoot ${CMAKE_SOURCE_DIR})
add_subdirectory(tool)
add_subdirectory(test)

set(cxxLox_src 
    src/Lox.cxx
    src/Scanner.cxx
    src/Token.cxx
)
add_executable(cxxLox
    ${cxxLox_src}
)


target_compile_options(cxxLox PUBLIC
    -g
    -std=c++17
    -fsanitize=address
)

target_include_directories(cxxLox PUBLIC
    ./header
    ./utils
)

target_link_libraries(cxxLox
    pthread
    asan.a
    dl
    rt
    fmt.a
)