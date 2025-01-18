cmake.exe -B Build\\CC . -G"MinGW Makefiles" -D CMAKE_EXPORT_COMPILE_COMMANDS=ON -D CMAKE_BUILD_TYPE=Release
xcopy /y Build\\CC\\compile_commands.json Build\\compile_commands.json
