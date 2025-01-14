@echo off
cls

Rem Build
if not exist Build\Release mkdir Build\Release\Run
cd Build\Release
cmake.exe -S ..\..\ -B . -G"MinGW Makefiles" -D CMAKE_EXPORT_COMPILE_COMMANDS=ON -D CMAKE_BUILD_TYPE=Release
C:\Users\gerku\Documents\mingw64\bin\mingw32-make.exe
xcopy /y compile_commands.json ..\compile_commands.json

cd Run
echo n | copy /-y "C:/Users/gerku/Documents/netCDF 4.9.2/bin"\netcdf.dll .
echo n | copy /-y "C:/Users/gerku/Documents/netCDF 4.9.2/bin"\hdf5.dll .
echo n | copy /-y "C:/Users/gerku/Documents/netCDF 4.9.2/bin"\hdf5_hl.dll .

Rem Lauch
MyProject.exe
cd ..\..\..

