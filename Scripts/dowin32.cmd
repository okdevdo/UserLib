@echo off
if "%1" == "" goto :usage
md "UserLib\vs\%1\Debug"
md "UserLib\vs\%1\Release"
cd bzip2-1.0.6
call :makedir bzip2.lib %1 %2
cd ..
cd expat-2.1.0\lib
call :makedir2 expat.lib %1 %2
cd ..\..
cd sqlite-autoconf-3080403
call :makedir sqlite3.lib %1 %2
cd ..
cd xz-5.0.5\windows
call :makedir2 xz.lib %1 %2
cd ..\..
cd zlib-1.2.8
call :makedir3 zlib.lib %1 %2
cd ..
goto :EOF
:makedir
if "%3" == "" goto :makedir0
nmake -f makefile-ok.msc clean
:makedir0
nmake -f makefile-ok.msc
if errorlevel 1 goto :EOF
copy "%1" "..\UserLib\vs\%2\Debug"
copy "%1" "..\UserLib\vs\%2\Release"
goto :EOF
:makedir2
if "%3" == "" goto :makedir9
nmake -f makefile-ok.msc clean
:makedir9
nmake -f makefile-ok.msc
if errorlevel 1 goto :EOF
copy "%1" "..\..\UserLib\vs\%2\Debug"
copy "%1" "..\..\UserLib\vs\%2\Release"
goto :EOF
:makedir3
if "%3" == "" goto :makedir8
nmake -f makefile-ok.msc clean
:makedir8
nmake -f makefile-ok.msc LOC="-DASMV -DASMINF" OBJA="inffas32.obj match686.obj"
if errorlevel 1 goto :EOF
copy "%1" "..\UserLib\vs\%2\Debug"
copy "%1" "..\UserLib\vs\%2\Release"
goto :EOF
:usage
echo dowin32 'version' [clean]
