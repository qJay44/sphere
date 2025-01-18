@echo off
cls

Rem Build
if not exist Build mkdir Build
cd Build
cmake -S .. -B . -G "Visual Studio 17 2022"
cmake --build . --config Release

Rem Copy dlls
cd Release
echo n | copy /-y "C:\\Users\\q44\\Documents\\Libs\\netCDF 4.9.2\\bin\\netcdf.dll" .
echo n | copy /-y "C:\\Users\\q44\\Documents\\Libs\\netCDF 4.9.2\\bin\\hdf5.dll" .
echo n | copy /-y "C:\\Users\\q44\\Documents\\Libs\\netCDF 4.9.2\\bin\\hdf5_hl.dll" .
echo n | copy /-y "C:\\Users\\q44\\Documents\\Libs\\netCDF 4.9.2\\bin\\libcurl.dll" .

Rem Lauch
MyProject.exe
cd ..\..

