@echo off


cd /D %~dp0


REM This generate one folder containing all files that will be used for Downloading.


REM Make Output Directory
rmdir Output /s/q
mkdir Output
cd Output
mkdir Include
mkdir Library
mkdir App
cd ..

REM Included Files
copy Common\LogClient.h Output\Include

REM Library Files
copy Bin\Debug\dLogClient.lib Output\Library
copy Bin\Debug\sdLogClient.lib Output\Library
copy Bin\Release\rLogClient.lib Output\Library
copy Bin\Release\srLogClient.lib Output\Library

REM App
copy Bin\Release\LogView.exe Output\App