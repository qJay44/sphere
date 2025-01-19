@echo off
cls

Rem Build
cmake -S . -B Build -G "Visual Studio 17 2022"
cmake --build Build --config Release

Rem Copy dlls
cd Build\\Release
echo n | copy /-y "C:\\Users\\q44\\Documents\\Libs\\netCDF 4.9.2\\bin\\netcdf.dll" .
echo n | copy /-y "C:\\Users\\q44\\Documents\\Libs\\netCDF 4.9.2\\bin\\hdf5.dll" .
echo n | copy /-y "C:\\Users\\q44\\Documents\\Libs\\netCDF 4.9.2\\bin\\hdf5_hl.dll" .
echo n | copy /-y "C:\\Users\\q44\\Documents\\Libs\\netCDF 4.9.2\\bin\\libcurl.dll" .
echo n | copy /-y "C:\\Users\\q44\\Documents\\Libs\\tiff-4.7.0\\install\\bin\\tiff.dll" .

Rem Lauch
MyProject.exe
cd ..\..

