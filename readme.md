# for better develop
generate compile db file
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=YES -Bbuild
cmake --build . --target cxxLox